//
// Created by Moyu on 16/10/9.
//

#include <nan.h>

#include "../cpp_src/server.h"
#include "../cpp_src/client.h"

bool ValidArgsNum(int n, const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if (info.Length() < n) {
        Nan::ThrowTypeError("Wrong number of arguments");
        return false;
    }m
    return true;
}

void ClientRun(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if(!ValidArgsNum(2, info)){
        return;
    }
    if (!info[0]->IsString() || !info[1]->IsNumber()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    v8::String::Utf8Value str(info[0]->ToString());

    char* arg0 = *str;
    int arg1 = info[1]->NumberValue();

    client_run(arg0, arg1);
}

void ServerRun(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    if(!ValidArgsNum(2, info)){
        return;
    }
    if (!info[0]->IsString() || !info[1]->IsNumber()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    v8::String::Utf8Value str(info[0]->ToString());

    char* arg0 = *str;
    int arg1 = info[1]->NumberValue();

    server_run(arg0, arg1);
}

void Init(v8::Local<v8::Object> exports) {
    exports->Set(Nan::New("clientrun").ToLocalChecked(),
                 Nan::New<v8::FunctionTemplate>(ClientRun)->GetFunction());

    exports->Set(Nan::New("serverrun").ToLocalChecked(),
                 Nan::New<v8::FunctionTemplate>(ServerRun)->GetFunction());
}

NODE_MODULE(ctcp, Init)
