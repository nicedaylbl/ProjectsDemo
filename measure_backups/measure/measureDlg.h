
// measureDlg.h : 头文件
//
#pragma once
#include "ChartCtrl/ChartCtrl.h"
#include "MyStatic.h"
#include "visa.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "vector"

using namespace std;
#define  MAXNUM 1000
#define  MAXDWPONIT 100 //可视窗口最大点数
#define  CMD_BUFFER_SIZE 512
#define VECTOR vector<pair<double, CString>>
// CmeasureDlg 对话框
class CmeasureDlg : public CDialog
{
// 构造
public:
	CmeasureDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MEASURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CChartCtrl myChartCtrl;//画图控件
	CString strGpib,strPowerAddress,strVoltage,strSample;//界面获取参数
	CChartLineSerie* m_pLineSerie; //曲线绘图指针
	VECTOR *myCurrentData;//测量电流存储vector
	CMyStatic myTotalTime,myRmsCurrent;//显示测试总时间和电流文本框的设置
	CFont myStaticResultFont;//字体
	ViStatus viStatus;
	ViSession DeviceHandle,m_viDefaultRM;
	ViChar szCmdBuff[CMD_BUFFER_SIZE];

	CEdit m_GPIB;
	CEdit m_addPower;
	CEdit m_voltage;
	CEdit m_rate;
	CListCtrl myListCtrl;
	CStatic m_StaticAxis;

	double myCurrentArray[MAXNUM];//绘图所用数组
	double   mySampleTime[MAXNUM];
	int    myCount;//采样次数
	double myrealCurrent;//单次采样电流值
	double myrealVoltage;//单次测量电压值
	int mySampleRate;
	int maxLeftAxis;//纵坐标最大值
	CChartAxis *pLeftAxis,*pBottomAxis;	
	BOOL isDrawFlag;
	//List控件显示结果
	int totalTimeCount;//累计时间
	double sumCurrent;//计算平均电流大小
	double maxCurrent,minCurrent;//最大最小电流
	SYSTEMTIME st;

public:
	afx_msg void OnBnClickedButtonSavedata();
	afx_msg void OpenDevice();
	afx_msg void OnSetBottomtAxisAuto(int rate);
	afx_msg void OnDrawMoving();	
	afx_msg void LeftMoveArray(double* ptrCurrent,double* ptrSample,size_t length,double data);
	afx_msg void FillCurrentArray( double data);
	afx_msg void SetTotalTime();
	afx_msg void CurrentMeasures();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void SetListResult();
	afx_msg void CloseDevice();
	afx_msg void OnBnClickedButtonOpendivece();
	afx_msg void OnBnClickedButtonQuit();
};

class CMyCursorListener : public CChartCursorListener
{
public:
	void OnCursorMoved(CChartCursor *pCursor, double xValue, double yValue)
	{
		CmeasureDlg * pThis = NULL;
		pThis = (CmeasureDlg*)AfxGetApp()->m_pMainWnd;	
		CString tempstr =_T("");
		CString bottomaAxisUnit =_T("");
		double bottomAxis = 0;
		if ( xValue > 1000 )
		{
			bottomaAxisUnit = _T('s');
			bottomAxis = xValue/1000;
		}
		else
		{
			bottomAxis = xValue;
			bottomaAxisUnit = _T("ms");
		}							
		tempstr.Format(_T("时间:%3.3f%s;电流:%3.3fmA"),bottomAxis ,bottomaAxisUnit.GetBuffer(),yValue);
		pThis->m_StaticAxis.SetWindowText(tempstr);	
	}
};

