// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 CHAPTER51DLL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// CHAPTER51DLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef CHAPTER51DLL_EXPORTS
#define CHAPTER51DLL_API __declspec(dllexport)
#else
#define CHAPTER51DLL_API __declspec(dllimport)
#endif

// 此类是从 dll 导出的
class CHAPTER51DLL_API Cchapter51dll {
public:
	Cchapter51dll(void);
	int add(int i, int j);
	int sub(int i, int j);
};

extern "C" CHAPTER51DLL_API double calc_pi(void);
