
// measureDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "measure.h"
#include "measureDlg.h"
#include "ChartCtrl/ChartAxisLabel.h"
#include "ChartCtrl/ChartLineSerie.h"
#include <stdlib.h>
#include <string.h>
#include <locale>
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CmeasureDlg 对话框




CmeasureDlg::CmeasureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CmeasureDlg::IDD, pParent)
{	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	viStatus = VI_FALSE;
	memset(myCurrentArray,0,sizeof(myCurrentArray));
	memset(mySampleTime,0, sizeof(mySampleTime));
	totalTimeCount = 0;
	myrealCurrent = 0;
	myCount = 1;
	maxCurrent = 0;
	minCurrent = 0;
	sumCurrent = 0;
}

void CmeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHART, myChartCtrl);	
	DDX_Control(pDX, IDC_STATIC_TOTALTIME, myTotalTime);
	DDX_Control(pDX, IDC_STATIC_RMSCURRENT,myRmsCurrent);
	DDX_Control(pDX, IDC_EDIT_GPIBADD2, m_GPIB);
	DDX_Control(pDX, IDC_EDIT_POWADD, m_addPower);
	DDX_Control(pDX, IDC_EDIT_VOLTAGE, m_voltage);
	DDX_Control(pDX, IDC_EDIT_SAMPLE, m_rate);
	DDX_Control(pDX, IDC_LIST_RESULT, myListCtrl);
}

BEGIN_MESSAGE_MAP(CmeasureDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_BUTTON_SAVEDATA, &CmeasureDlg::OnBnClickedButtonSavedata)
	ON_BN_CLICKED(IDC_BUTTON_START, &CmeasureDlg::OnBnClickedButtonStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_OPENDIVECE, &CmeasureDlg::OnBnClickedButtonOpendivece)
	ON_BN_CLICKED(IDC_BUTTON_QUIT, &CmeasureDlg::OnBnClickedButtonQuit)
END_MESSAGE_MAP()


// CmeasureDlg 消息处理程序

BOOL CmeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	pThread = NULL;
	myCurrentData = new VECTOR;

	m_GPIB.SetWindowText(_T("0"));
	m_addPower.SetWindowText(_T("5"));
	m_voltage.SetWindowText(_T("3.8"));
	m_rate.SetWindowText(_T("1"));	


	CButton* radio=(CButton*)GetDlgItem(IDC_RADIO_CURRENT);
	radio->SetCheck(1);

	// TODO: 在此添加额外的初始化代码
	//初始化曲线表
	CChartAxis *pAxis= NULL;
	pAxis = myChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	pAxis->SetAutomatic(false);
	pAxis->SetMinMax(0,2);
	pAxis->EnableScrollBar(true);
	pAxis->SetPanZoomEnabled(false);
	pAxis = myChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(false);
	pAxis->EnableScrollBar(true);
	//pAxis->SetAxisToScrollStep()
	pAxis->SetMinMax(0,100);
	//pAxis->SetPanZoomEnabled(false);

	TChartString strtitle = _T("功耗图表");
	myChartCtrl.GetTitle()->AddString(strtitle);
	TChartString strLeftAxis = _T("电流（A）");
	TChartString strBottomAxis = _T("时间（s）");
	myChartCtrl.GetLeftAxis()->GetLabel()->SetText(strLeftAxis);
	myChartCtrl.GetBottomAxis()->GetLabel()->SetText(strBottomAxis);
	m_pLineSerie = myChartCtrl.CreateLineSerie();
   
	//// Creates a cross-hair cursor associated with the two primary axes.
	//CChartCrossHairCursor* pCrossHair = myChartCtrl.CreateCrossHairCursor();
	// //Creates a dragline cursor associated with the bottom axis.
	//CChartDragLineCursor* pDragLine = myChartCtrl.CreateDragLineCursor(CChartCtrl::BottomAxis);
	//// Hides the mouse when it is over the plotting area.
	//myChartCtrl.ShowMouseCursor(false);


	myStaticResultFont.CreateFont(30,0,0,0,FW_NORMAL,FALSE,FALSE,0,ANSI_CHARSET,\
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,_T("Arial"));
	myRmsCurrent.SetFont(&myStaticResultFont);
	myRmsCurrent.SetBKColor(RGB(0,150,0));
	myRmsCurrent.SetTextColor(RGB(0,0,255));
	myRmsCurrent.SetWindowText(_T("0.0mA"));

	myTotalTime.SetFont(&myStaticResultFont);
	myTotalTime.SetBKColor(RGB(0,150,0));
	myTotalTime.SetTextColor(RGB(0,0,255));
	myTotalTime.SetWindowText(_T("00时00分00秒"));

	//设置myListResult结果显示	
	DWORD dwStyle =myListCtrl.GetExtendedStyle();
	dwStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES ;
	myListCtrl.SetExtendedStyle(dwStyle);//设置扩展风格	
	myListCtrl.InsertColumn(0,_T("指标"),LVCFMT_CENTER,80);
	myListCtrl.InsertColumn(1,_T("数值(mA)"),LVCFMT_CENTER,110);
	myListCtrl.InsertItem(0,_T("当前电流值"));
	myListCtrl.InsertItem(1,_T("平均电流值"));
	myListCtrl.InsertItem(2,_T("最大电流值"));
	myListCtrl.InsertItem(3,_T("最小电流值"));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CmeasureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CmeasureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CmeasureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CmeasureDlg::OnBnClickedButtonSavedata()
{
	// TODO: 在此添加控件通知处理程序代码
	OPENFILENAME  ofn;	 
	TCHAR szFile[MAX_PATH];
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = _T('\0');
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _T("Text\0*.txt\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
	CString strFile;
	if (GetSaveFileName(&ofn))
	{	
		strFile.Format(_T("%s"),szFile);
	}
	else
		return;
	if (strFile.Find(_T('.'))!= -1)	
	{	
		strFile = strFile.Left(strFile.Find(_T('.')));	
	}	

	strFile+=_T(".txt");
	CStdioFile File;
	CFileException fileException;
	if (File.Open(strFile,CFile::modeCreate | CFile::modeReadWrite | CFile::typeText))
	{	
		char* old_local = _strdup(setlocale(LC_CTYPE,NULL));
		setlocale(LC_CTYPE, "chs");
		File.WriteString(_T("序号  采样时间(s)  电流(mA)\n"));
	} 
	int testNumber=0;
	VECTOR::iterator itt = myCurrentData->begin();	
	CString bufferData = _T("");	
	while( itt != myCurrentData->end())
	{
		double tempCurret = (*itt).first;
		CString tempCurrentTime = (*itt).second; 
		bufferData.Format(_T("%d    %s    %.3f\n"),testNumber+1,tempCurrentTime,tempCurret);
		File.WriteString(bufferData);
		++testNumber;
		*itt++;
	}
	File.Close();
}

ViStatus CmeasureDlg::OpenDevice()
{	
	viStatus = VI_SUCCESS;
	m_GPIB.GetWindowText(strGpib);
	m_addPower.GetWindowText(strPowerAddress);
	m_voltage.GetWindowText(strVoltage);
	double Voltage = _wtof(strVoltage.GetBuffer(0));
	if (VI_SUCCESS > viOpenDefaultRM(&m_viDefaultRM) )
	{
		AfxMessageBox(_T("打开电源DefaultRM失败！"));
		//return VI_FALSE;
	}
	else
	{
		sprintf(szCmdBuff,"GPIB%d::%d::INSTR",_ttoi(strGpib),_ttoi(strPowerAddress));
		viStatus = viOpen(m_viDefaultRM, szCmdBuff, VI_NULL, VI_NULL, &DeviceHandle);
		if (!viStatus)
		{
			viStatus |= viSetAttribute(DeviceHandle,VI_ATTR_TMO_VALUE,20000);
			viStatus |= viQueryf(DeviceHandle,"*IDN?\n","%t",szCmdBuff);
			viStatus |= viPrintf(DeviceHandle,"*RST\n");
			viStatus |= viPrintf(DeviceHandle,"INST:COUP:OUTP:STAT NONE\n");//指令只对其中某一路生效。
			viStatus |= viPrintf(DeviceHandle,"VOLT %.5f;CURR 3\n",Voltage);			
			viStatus |= viPrintf(DeviceHandle,"OUTP ON\n");			
		}		
		else
		{
			AfxMessageBox(_T("打开电源失败！"));
		}		
	}
	return viStatus;
}

void CmeasureDlg::OnSetBottomtAxisAuto(int rate)
{
	//rate单位为ms
	CChartAxis *pAxis = NULL;
	int tempSample = rate >= 1000 ? (rate/1000) : rate;
	pAxis = myChartCtrl.GetBottomAxis();
	if ( myCount <= MAXDWPONIT )
	{
		pAxis->SetMinMax(0,MAXDWPONIT*tempSample);
	}
	else
	{
		int startAxis = ((myCount-1)%MAXDWPONIT + (myCount-1)/MAXDWPONIT)*tempSample;
		int endAxis = ((myCount-1)%MAXDWPONIT+MAXDWPONIT + (myCount-1)/MAXDWPONIT)*tempSample;
		pAxis->SetMinMax(startAxis,endAxis);
	}	
}

void CmeasureDlg::OnDrawMoving()
{
	int number = myCount<MAXNUM  ?myCount:MAXNUM;	
	m_pLineSerie->ClearSerie();	
	if ( myCount < MAXNUM )
	{
		FillCurrentArray(myrealCurrent);
	}	
	if ( myCount >= MAXNUM )
	{
		LeftMoveArray(myCurrentArray,mySampleTime,number,myrealCurrent);
	}
	OnSetBottomtAxisAuto(mySampleRate);
	//LeftMoveArray(myCurrentArray,mySampleTime,number,myCurrentOnce);	
	m_pLineSerie->AddPoints(mySampleTime,myCurrentArray,number);
}

void CmeasureDlg::LeftMoveArray(double* ptrCurrent,double* ptrSample,size_t length,double data)
{
	for (size_t i=1;i<length;++i)
	{
		ptrCurrent[i-1] = ptrCurrent[i];
	}
	ptrCurrent[length-1] = data;

	for (size_t j=1;j<length;++j)
	{
		ptrSample[j-1] = ptrSample[j];
	}
	ptrSample[length-1] =myCount*mySampleRate ;

}

void CmeasureDlg::FillCurrentArray( double data)
{
	int temp = mySampleRate >= 1000 ? mySampleRate/1000 : mySampleRate;
	myCurrentArray[myCount-1] = data;
	mySampleTime[myCount-1]   =myCount*temp;	
}

void CmeasureDlg::SetTotalTime()
{
	int hour = 0 ;
	int minute = 0;
	int second = 0;	
	CString temptotal = _T("");	
	while (showTimeFlag)
   {
	   Sleep(1000);
	   ++totalTimeCount;
	   hour = totalTimeCount/3600;
	   minute = (totalTimeCount%3600)/60;
	   second = (totalTimeCount%3600)%60;
	   temptotal.Format(_T("%02d时%02d分%02d秒"),hour,minute,second);
	   myTotalTime.SetWindowText(temptotal);
   }
}

void CmeasureDlg::SetListResult()
{
	CString itemText = _T("");
	itemText.Format(_T("%03.3f mA"),sumCurrent/myCount);
	myRmsCurrent.SetWindowText(itemText);
	itemText.Format(_T("%.5f"),myrealCurrent);
	myListCtrl.SetItemText(0,1,itemText);
	itemText.Format(_T("%.5f"),sumCurrent/myCount);
	myListCtrl.SetItemText(1,1,itemText);
	itemText.Format(_T("%.5f"),maxCurrent);
	myListCtrl.SetItemText(2,1,itemText);
	itemText.Format(_T("%.5f"),minCurrent);
	myListCtrl.SetItemText(3,1,itemText);
}

ViStatus CmeasureDlg::CurrentMeasures()
{
	viStatus = VI_SUCCESS;
	viStatus |= viQueryf(DeviceHandle,"MEAS:CURR?\n","%t",szCmdBuff);
	if (!viStatus)
	{
		stringstream cmd;
		cmd << szCmdBuff[0];
		cmd >> myrealCurrent;
	}
	return viStatus;
}
void CmeasureDlg::OnBnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	double voltage = 0;
	double current = 0;
	m_GPIB.GetWindowText(strGpib);
	m_addPower.GetWindowText(strPowerAddress);
	m_voltage.GetWindowText(strVoltage);
	m_rate.GetWindowText(strSample);
	mySampleRate = _ttoi(strSample);//采样时间为ms	
	//OnSetBottomtAxisAuto(_ttoi(strSample));
	str = mySampleRate >= 1000 ? _T("时间（s）") : _T("时间（ms）");
	myChartCtrl.GetBottomAxis()->GetLabel()->SetText(str.GetBuffer(0));
	GetDlgItem(IDC_BUTTON_START)->GetWindowText(str);

	if (!str.Compare(_T("开始")))
	{
		showTimeFlag = true;
		myCurrentData->clear();//
		GetDlgItem(IDC_BUTTON_START)->SetWindowText(_T("结束"));
		DWORD begin = GetTickCount();
		pThread = ::AfxBeginThread( ThreadFunForCLOCK,this, 0, 0, 0, NULL);
		SetTimer(1,mySampleRate,NULL);//采样率
		//SetTimer(2,1000,NULL);//该计时器记录测试总时间ms为单位。
	}

	if (!str.Compare(_T("结束")))
	{
		showTimeFlag = false;
		GetDlgItem(IDC_BUTTON_START)->SetWindowText(_T("开始"));
		//结束之后重置测试相关的全局变量，以便开始下次测试。	
		memset(myCurrentArray,0,sizeof(myCurrentArray));
		memset(mySampleTime,0, sizeof(mySampleTime));
		totalTimeCount = 0;
		myrealCurrent = 0;
		myCount = 1;
		maxCurrent = 0;
		minCurrent = 0;
		sumCurrent = 0;
		KillTimer(1);
		KillTimer(2);
	}
}

void CmeasureDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (1 == nIDEvent)
	{
		DWORD begin2 = GetTickCount();
		CString strSystime;
		GetLocalTime(&st);
		strSystime.Format(_T("%02d-%02d_%02d:%02d:%02d:%02d"),st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);	
		//myrealCurrent = 1000*myrealCurrent;//直接取出来电流结果单位为A
		strSystime.ReleaseBuffer();
		if (!CurrentMeasures())
		{
			myrealCurrent = 0;
		}		
		myCurrentData->push_back(make_pair(myrealCurrent,strSystime));
		maxCurrent = maxCurrent > myrealCurrent ? maxCurrent:myrealCurrent;
		minCurrent = ( minCurrent < myrealCurrent ) && ( minCurrent > 0.000001)  ? minCurrent:myrealCurrent;
		sumCurrent = sumCurrent + myrealCurrent;
		DWORD begin3 = GetTickCount();
		SetListResult();
		OnDrawMoving();
		DWORD begin4 = GetTickCount();
		myCount++;
	}
	/*if ( 2 == nIDEvent)
	{
		DWORD begin5 = GetTickCount();
		SetTotalTime();
		DWORD begin6 = GetTickCount();
		DWORD t = begin6 - begin5;
	}*/
	CDialog::OnTimer(nIDEvent);
}


void CmeasureDlg::OnBnClickedButtonOpendivece()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	GetDlgItem(IDC_BUTTON_OPENDIVECE)->GetWindowText(str);
	if (!str.Compare(_T("打开电源")))
	{			
		if (!OpenDevice())
		{
			GetDlgItem(IDC_BUTTON_OPENDIVECE)->SetWindowText(_T("关闭电源"));			
		}			
	}
	if (!str.Compare(_T("关闭电源")))
	{
		GetDlgItem(IDC_BUTTON_OPENDIVECE)->SetWindowText(_T("打开电源"));
		CloseDevice();
	}
}

void CmeasureDlg::OnBnClickedButtonQuit()
{
	// TODO: 在此添加控件通知处理程序代码
	delete myCurrentData;
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}

void CmeasureDlg::CloseDevice()
{
	if (!viStatus)
	{
		viPrintf(DeviceHandle,"OUTP OFF\n");		
	}
	viClose(m_viDefaultRM);
	viClose(DeviceHandle);
}

UINT CmeasureDlg::ThreadFunForCLOCK(LPVOID pParam)
{
	CmeasureDlg *dpThis;
	dpThis = (CmeasureDlg *)pParam;
	CmeasureDlg* pMF=(CmeasureDlg*)AfxGetApp()->m_pMainWnd;
	dpThis->m_MainWindowHandle = pMF->GetSafeHwnd();
	dpThis->SetTotalTime();
	return 1;
}
