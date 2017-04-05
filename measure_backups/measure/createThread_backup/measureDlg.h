
// measureDlg.h : ͷ�ļ�
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
#define  MAXDWPONIT 50
#define  CMD_BUFFER_SIZE 512
#define VECTOR vector<pair<double, CString>>
// CmeasureDlg �Ի���
class CmeasureDlg : public CDialog
{
// ����
public:
	CmeasureDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MEASURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CChartCtrl myChartCtrl;//��ͼ�ؼ�
	CString strGpib,strPowerAddress,strVoltage,strSample;//�����ȡ����
	CChartLineSerie* m_pLineSerie; //���߻�ͼָ��
	VECTOR *myCurrentData;//���������洢vector
	CMyStatic myTotalTime,myRmsCurrent;//��ʾ������ʱ��͵����ı��������
	CFont myStaticResultFont;//����
	ViStatus viStatus;
	ViSession DeviceHandle,m_viDefaultRM;
	ViChar szCmdBuff[CMD_BUFFER_SIZE];

	CEdit m_GPIB;
	CEdit m_addPower;
	CEdit m_voltage;
	CEdit m_rate;
	CListCtrl myListCtrl;
	
	double myCurrentArray[MAXNUM];//��ͼ��������
	double   mySampleTime[MAXNUM];
	int    myCount;//��������
	double myrealCurrent;//���β�������ֵ
	double myrealVoltage;//���β�����ѹֵ
	int mySampleRate;
	//List�ؼ���ʾ���
	int totalTimeCount;//�ۼ�ʱ��
	double sumCurrent;//����ƽ��������С
	double maxCurrent,minCurrent;//�����С����
	BOOL showTimeFlag;
	SYSTEMTIME st;
    CWinThread *pThread;
	HWND m_MainWindowHandle;
public:
	afx_msg void OnBnClickedButtonSavedata();
	afx_msg ViStatus OpenDevice();
	afx_msg void OnSetBottomtAxisAuto(int rate);
	afx_msg void OnDrawMoving();	
	afx_msg void LeftMoveArray(double* ptrCurrent,double* ptrSample,size_t length,double data);
	afx_msg void FillCurrentArray( double data);
	afx_msg void SetTotalTime();
	afx_msg ViStatus CurrentMeasures();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void SetListResult();
	afx_msg void CloseDevice();
	afx_msg void OnBnClickedButtonOpendivece();
	afx_msg void OnBnClickedButtonQuit();

private:
	static UINT ThreadFunForCLOCK(LPVOID pParam);
};

