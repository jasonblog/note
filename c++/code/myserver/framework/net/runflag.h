#ifndef RUNFLAG_H_
#define RUNFLAG_H_


// 服务器状态
enum ERunFlag
{
	ERF_RUNTIME = 0,	// 运行中
	ERF_QUIT,			// 退出
	ERF_RELOAD,			// 重新加载配置
};

// 控制服务器运行状态的类
class CRunFlag
{
public:
	CRunFlag();
	~CRunFlag();

	void SetRunFlag(ERunFlag eRunFlag);
	bool CheckRunFlag(ERunFlag eRunFlag);

protected:
	ERunFlag mRunFlag;
};


#endif /* RUNFLAG_H_ */
