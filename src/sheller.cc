#include <v8.h>
#include <node.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <libgen.h>
#include <string.h>

using namespace v8;
using namespace node;

char *get(v8::Local<v8::Value> value, const char *fallback = "") {
    if (value->IsString()) {
        v8::String::Utf8Value string(value);
        char *str = (char *) malloc(string.length() + 1);
        strcpy(str, *string);
        return str;
    }
    char *str = (char *) malloc(strlen(fallback) + 1);
    strcpy(str, fallback);
    return str;
}

static Handle<Value> Shell(const Arguments& args) {
    HandleScope scope;

    if (args[0]->IsString()) {
        //piping returns from the children through these
        int inpipe[2];
        int outpipe[2];
        int errpipe[2];
        //pipe, not bothering with pedantic error handling as if this fails
        //there is no way to recover, it'll just nag you
        pipe(inpipe);
        pipe(outpipe);
        pipe(errpipe);

        int pid = fork();
        if (pid < 0) {
            return ThrowException(Exception::Error(String::New("Failed to fork process")));
        } else if(pid) {
            //parent
            close(inpipe[0]);
            close(outpipe[1]);
            close(errpipe[1]);
            //reading takes place here, this is a bit doubled up on the read
            //avoid duplicating the rest of the function to support returning
            //the string value of a shelled command
            bool silenceStdOut = args.Length() > 1 && args[1]->IsTrue();
            bool silenceStdErr = false;
            const int bufferSize = 1024;
            char buffer[bufferSize];
            char *returnStringBuffer = 0;
            int totalBytesRead = 0;
            int bytesRead = 0;
            //stdout
            while((bytesRead = read(outpipe[0], &buffer, bufferSize)) > 0) {
                if (!silenceStdOut) {
                    write(STDOUT_FILENO, &buffer, bytesRead);
                }
                returnStringBuffer =
                    (char*)realloc(returnStringBuffer, totalBytesRead + bytesRead);
                if (returnStringBuffer) {
                    memmove(&returnStringBuffer[totalBytesRead], buffer, bytesRead);
                    totalBytesRead += bytesRead;
                } else {
                    //might as well exit, there isn't enough memory to even
                    //make an exception
                    exit(2);
                }
            }
            Local<String> stdoutString = String::New(returnStringBuffer, totalBytesRead);
            free(returnStringBuffer);
            returnStringBuffer = 0;
            totalBytesRead = 0;
            while((bytesRead = read(errpipe[0], &buffer, bufferSize)) > 0) {
                if (!silenceStdErr) {
                    write(STDERR_FILENO, &buffer, bytesRead);
                }
                returnStringBuffer =
                    (char*)realloc(returnStringBuffer, totalBytesRead + bytesRead);
                if (returnStringBuffer) {
                    memmove(&returnStringBuffer[totalBytesRead], buffer, bytesRead);
                    totalBytesRead += bytesRead;
                } else {
                    exit(2);
                }
            }
            Local<String> stderrString = String::New(returnStringBuffer, totalBytesRead);
            free(returnStringBuffer);
            //wait for the child to exit, this makes the world synchronou
            int r, status;
            do {
                r = waitpid(pid, &status, WNOHANG);
            } while (r != -1);
            close(inpipe[1]);
            close(outpipe[0]);
            close(errpipe[0]);
            //at this point, we are exited, come back with result codes
            Local<Object> result = Object::New();
            result->Set(String::New("pid"), Integer::New(pid));
            result->Set(String::New("stdout"), stdoutString);
            result->Set(String::New("stderr"), stderrString);
            if (WIFEXITED(status)) {
                result->Set(String::New("exitCode"), Integer::New(WEXITSTATUS(status)));
                result->Set(String::New("signalCode"), Null());
                return scope.Close(result);
            }
            else if (WIFSIGNALED(status)) {
                result->Set(String::New("exitCode"), Null());
                result->Set(String::New("signalCode"), Integer::New(WTERMSIG(status)));
                return scope.Close(result);
            } else {
                //this is to cover the if, we don't get here
                assert(false);
            }
        } else {
            //child
            //unbuffered IO, we'll go blockwise in the parent
            setvbuf(stdout, (char*)NULL, _IONBF, 0);
            //move out and err to the pipes, again without the pedantic
            //error handling for which there is no recovery option
            dup2(inpipe[0], STDIN_FILENO);
            dup2(outpipe[1], STDOUT_FILENO);
            dup2(errpipe[1], STDERR_FILENO);
            close(inpipe[1]);
            close(outpipe[0]);
            close(errpipe[0]);
            close(inpipe[0]);
            close(outpipe[1]);
            close(errpipe[1]);
            String::Utf8Value command(args[0]);
            if (args.Length() > 2 && args[2]->IsArray()) {
                //arguments were specified, let's use this as a straight
                //command rather than as a subshell
                Local<Array> commandArguments = Local<Array>::Cast(args[2]);
                char * argv[commandArguments->Length() + 2];
                argv[0] = *command;
                for (uint i=0; i < commandArguments->Length(); i++){
                    argv[i+1] = get(commandArguments->Get(i));
                }
                argv[commandArguments->Length() + 1] = NULL;
                int err = execvp(*command, argv);
                //we are going to exit rightaway, one way or another so no
                //worries about freeing argv
                //should not get here
                exit(err);
            } else {
                //arguments were specified, let's use this as a straight
                //your-shell-here, this is the main 'improvement' over system(3)
                if (char *shell = getenv("SHELL")) {
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

NODE_MODULE(sheller, init)
