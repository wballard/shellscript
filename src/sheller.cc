#include <v8.h>
#include <node.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <libgen.h>

using namespace v8;
using namespace node;

static Handle<Value> Shell(const Arguments& args) {
    HandleScope scope;

    if (args[0]->IsString()) {
        //piping returns from the children through these
        int outpipe[2];
        int errpipe[2];
        //pipe, not bothering with pedantic error handling as if this fails
        //there is no way to recover, it'll just nag you
        pipe(outpipe);
        pipe(errpipe);

        int pid = fork();
        if (pid < 0) {
            return ThrowException(Exception::Error(String::New("Failed to fork process")));
        } else if(pid) {
            //parent
            //drain the piped back IO
            close(outpipe[1]);
            close(errpipe[1]);
            char buf;
            const int len = 1;
            while(read(outpipe[0], &buf, len) == len) {
                write(STDOUT_FILENO, &buf, len);
            }
            while(read(errpipe[0], &buf, len) == len) {
                write(STDERR_FILENO, &buf, len);
            }
            //wait for the child to exit, this makes the world synchronou
            int r, status;
            do {
                r = waitpid(pid, &status, WNOHANG);
            } while (r != -1);
            Local<Object> result = Object::New();
            if (WIFEXITED(status)) {
                result->Set(String::New("pid"), Integer::New(pid));
                result->Set(String::New("exitCode"), Integer::New(WEXITSTATUS(status)));
                result->Set(String::New("signalCode"), Null());
                return scope.Close(result);
            }
            else if (WIFSIGNALED(status)) {
                result->Set(String::New("pid"), Integer::New(pid));
                result->Set(String::New("exitCode"), Null());
                result->Set(String::New("signalCode"), Integer::New(WTERMSIG(status)));
                return scope.Close(result);
            } else {
                return scope.Close(Undefined());
            }
        } else {
            //child
            //unbuffered IO, we'll go blockwise in the parent
            setvbuf(stdout, (char*)NULL, _IONBF, 0);
            //move out and err to the pipes, again without the pedantic
            //error handling for which there is no recovery option
            dup2(outpipe[1], STDOUT_FILENO);
            dup2(errpipe[1], STDERR_FILENO);
            close(outpipe[0]);
            close(errpipe[0]);
            v8::String::Utf8Value command(args[0]);
            //your-shell-here, this is the main 'improvement' over system(3)
            char *shell = getenv("SHELL");
            if (shell) {
                int err = execl(shell, basename(shell), "-c", *command, NULL);
                //should not get here
                exit(err);
            } else {
                //at this point, the call is pretty much good old system(3)
                int err = execl("/bin/sh", "sh", "-c", *command, NULL);
                //should not get here
                exit(err);
            }
        }
        //there really is no way to get here
        assert(false);
    } else {
        return ThrowException(Exception::Error(String::New("Must pass a string command line")));
    }
}

extern "C" void init(Handle<Object> exports) {
    HandleScope scope;
    NODE_SET_METHOD(exports, "shell", Shell);
}
