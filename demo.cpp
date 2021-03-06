#include "pscmd.h"
#include "windows.h"

int main()
{
	/* 以当前进程为例，应使用 OpneProcess 打开当前进程，而不是调用 GetCurrentProcess 获得伪句柄 */
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
	if (hProcess)
	{
		/* 
			进程句柄 hProcess 应具有 PROCESS_QUERY_INFORMATION |  PROCESS_VM_READ 访问权限，否则将导致 GetProcessCommandLine 失败。
			应用程序应尽可能使用 GetProcessCommandLineW 即 Unicode 版本，而不是 Ansi 版本，这有助于提高应用程序执行性能且防止在特定语言下丢失数据。
			pscmd.h header file 默认通过当前项目的字符集设置来确定所调用的 GetProcessCommandLine 版本与 PCMDBUFFER_T 缓冲区类型定义（Unicode 或 Ansi）。
		*/
		SIZE_T nCommandLineSize = NULL;
		if (GetProcessCommandLine(hProcess, NULL, NULL, &nCommandLineSize)) // 将 lpcBuffer 和 nSize 设置为 NULL 以获取建议缓冲区大小（nCommandLineSize）
		{
			/* 在堆上分配建议大小的 Unicode 缓冲区 */
			PCMDBUFFER_T lpUnicodeBuffer = (PCMDBUFFER_T)malloc(nCommandLineSize);
			if (lpUnicodeBuffer)
			{
				/* 使用 memset（或 WINAPI ZeroMemory）将分配的 Unicode 缓冲区初始化为 zero */
				memset(lpUnicodeBuffer, NULL, nCommandLineSize);
				// ZeroMemory(lpUnicodeBuffer, nCommandLineSize);

				/* 再次调用  GetProcessCommandLine 并传入所分配的 Unicode 缓冲区，以取得实际数据 */
				if (GetProcessCommandLine(hProcess, lpUnicodeBuffer, nCommandLineSize, &nCommandLineSize))
				{
					/* nCommandLineSize 的值当前为 GetProcessCommandLine 实际复制到 Uniocde 缓冲区 lpUnicodeBuffer 中的字节数，打印 lpUnicodeBuffer */
					wprintf_s(lpUnicodeBuffer);
				}

				/* 释放 Unicode 缓冲区 */
				free(lpUnicodeBuffer);
			}
		}

		/* 关闭进程句柄 */
		CloseHandle(hProcess);
	}
	return 0;
}
