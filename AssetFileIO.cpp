#include "AssetFileIO.h"

#ifdef EMSCRIPTEN
    #include <emscripten.h>

    void sys::LoadAssetComplete(const char *url, void *arg, onload_func onload, onerror_func onerror) {
        emscripten_async_wget_data(url, arg, onload, onerror);
    }
#else

#include <memory>

#include "AssetFile.h"
#include "ThreadWorker.h"

namespace {
    //sys::ThreadWorker worker;
}

void sys::LoadAssetComplete(const char *url, void *arg, onload_func onload, onerror_func onerror) {
	static sys::ThreadWorker worker;

    std::string url_str(url);
    worker.AddTask([url_str, arg, onload, onerror] {
#if 1
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
        AssetFile in_file(url_str.c_str(), AssetFile::IN);
		if (!in_file) {
			if (onerror) {
				onerror(arg);
			}
			return;
		}
        int size = (int) in_file.size();

        std::unique_ptr<char[]> buf(new char[size]);
        in_file.Read(&buf[0], size);

        if (onload) {
            onload(arg, &buf[0], size);
        }
    });
}

#endif