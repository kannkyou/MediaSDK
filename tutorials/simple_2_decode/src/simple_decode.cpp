// Copyright (c) 2019 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "common_utils.h"
#include "cmd_options.h"
#include "..\hevcd\hevcd.h"

int main()
{
    LPCTSTR DLL_HEVCD = "hevcd.dll";
    HINSTANCE hEVCD = ::LoadLibrary(DLL_HEVCD);
    if (hEVCD)
    {
        using dll_create_hevcd = decltype(&create_hevcd);
        using dll_destroy_hevcd = decltype(&destroy_hevcd);
        using dll_hd_decode = decltype(&hd_decode);
        using dll_hd_getframe = decltype(&hd_getframe);

        dll_create_hevcd create_hevcd = reinterpret_cast<dll_create_hevcd>(::GetProcAddress(hEVCD, "create_hevcd"));
        dll_destroy_hevcd destroy_hevcd = reinterpret_cast<dll_destroy_hevcd>(::GetProcAddress(hEVCD, "destroy_hevcd"));
        dll_hd_decode hd_decode = reinterpret_cast<dll_hd_decode>(::GetProcAddress(hEVCD, "hd_decode"));
        dll_hd_getframe hd_getframe = reinterpret_cast<dll_hd_getframe>(::GetProcAddress(hEVCD, "hd_getframe"));

        constexpr int BUF_LEN = 287982;
        LPBYTE buf = (LPBYTE)realloc(nullptr, BUF_LEN);

        fileUniPtr fSource(OpenFile(R"(D:\Kevin\moredata.h265)", "rb"), &CloseFile);
        fread(buf, 1, BUF_LEN, fSource.get());

        void* hdl = create_hevcd();
        LPBITMAPINFO lpbmpinfo = nullptr;
        LPBYTE frmdata = {};

        int i = 0;
        while (i++<50)
        {
            printf("i:%d\n", i);
            int ret = hd_decode(hdl, buf, BUF_LEN);
            printf("-->1\n");
            lpbmpinfo = nullptr;
            frmdata = hd_getframe(hdl, (void**)&lpbmpinfo);
            printf("-->2\n");
            //Sleep(20); // simulate render part code

            //destroy_hevcd(hdl);
            //printf("-->3\n");
            //hdl = create_hevcd();
            //printf("-->4\n");
        }

        //write result yuv file
        fileUniPtr fResult(OpenFile(R"(D:\Kevin\moredata.yuv)", "wb"), &CloseFile);
        fwrite(frmdata, 1, lpbmpinfo->bmiHeader.biSizeImage, fResult.get());

        destroy_hevcd(hdl);

        free(buf);

        ::FreeLibrary(hEVCD);
    }

    return 0;
}
