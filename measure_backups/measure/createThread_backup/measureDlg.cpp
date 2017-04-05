
// measureDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CmeasureDlg �Ի���




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


// CmeasureDlg ��Ϣ�������

BOOL CmeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	pThread = NULL;
	myCurrentData = new VECTOR;

	m_GPIB.SetWindowText(_T("0"));
	m_addPower.SetWindowText(_T("5"));
	m_voltage.SetWindowText(_T("3.8"));
	m_rate.SetWindowText(_T("1"));	


	CButton* radio=(CButton*)GetDlgItem(IDC_RADIO_CURRENT);
	radio->SetCheck(1);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//��ʼ�����߱�
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

	TChartString strtitle = _T("����ͼ��");
	myChartCtrl.GetTitle()->AddString(strtitle);
	TChartString strLeftAxis = _T("������A��");
	TChartString strBottomAxis = _T("ʱ�䣨s��");
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
	myTotalTime.SetWindowText(_T("00ʱ00��00��"));

	//����myListResult�����ʾ	
	DWORD dwStyle =myListCtrl.GetExtendedStyle();
	dwStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES ;
	myListCtrl.SetExtendedStyle(dwStyle);//������չ���	
	myListCtrl.InsertColumn(0,_T("ָ��"),LVCFMT_CENTER,80);
	myListCtrl.InsertColumn(1,_T("��ֵ(mA)"),LVCFMT_CENTER,110);
	myListCtrl.InsertItem(0,_T("��ǰ����ֵ"));
	myListCtrl.InsertItem(1,_T("ƽ������ֵ"));
	myListCtrl.InsertItem(2,_T("������ֵ"));
	myListCtrl.InsertItem(3,_T("��С����ֵ"));

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CmeasureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CmeasureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CmeasureDlg::OnBnClickedButtonSavedata()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		File.WriteString(_T("���  ����ʱ��(s)  ����(mA)\n"));
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
		AfxMessageBox(_T("�򿪵�ԴDefaultRMʧ�ܣ�"));
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
			viStatus |= viPrintf(DeviceHandle,"INST:COUP:OUTP:STAT NONE\n");//ָ��ֻ������ĳһ·��Ч��
			viStatus |= viPrintf(DeviceHandle,"VOLT %.5f;CURR 3\n",Voltage);			
			viStatus |= viPrintf(DeviceHandle,"OUTP ON\n");			
		}		
		else
		{
			AfxMessageBox(_T("�򿪵�Դʧ�ܣ�"));
		}		
	}
	return viStatus;
}

void CmeasureDlg::OnSetBottomtAxisAuto(int rate)
{
	//rate��λΪms
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
	   temptotal.Format(_T("%02dʱ%02d��%02d��"),hour,minute,second);
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	double voltage = 0;
	double current = 0;
	m_GPIB.GetWindowText(strGpib);
	m_addPower.GetWindowText(strPowerAddress);
	m_voltage.GetWindowText(strVoltage);
	m_rate.GetWindowText(strSample);
	mySampleRate = _ttoi(strSample);//����ʱ��Ϊms	
	//OnSetBottomtAxisAuto(_ttoi(strSample));
	str = mySampleRate >= 1000 ? _T("ʱ�䣨s��") : _T("ʱ�䣨ms��");
	myChartCtrl.GetBottomAxis()->GetLabel()->SetText(str.GetBuffer(0));
	GetDlgItem(IDC_BUTTON_START)->GetWindowText(str);

	if (!str.Compare(_T("��ʼ")))
	{
		showTimeFlag = true;
		myCurrentData->clear();//
		GetDlgItem(IDC_BUTTON_START)->SetWindowText(_T("����"));
		DWORD begin = GetTickCount();
		pThread = ::AfxBeginThread( ThreadFunForCLOCK,this, 0, 0, 0, NULL);
		SetTimer(1,mySampleRate,NULL);//������
		//SetTimer(2,1000,NULL);//�ü�ʱ����¼������ʱ��msΪ��λ��
	}

	if (!str.Compare(_T("����")))
	{
		showTimeFlag = false;
		GetDlgItem(IDC_BUTTON_START)->SetWindowText(_T("��ʼ"));
		//����֮�����ò�����ص�ȫ�ֱ������Ա㿪ʼ�´β��ԡ�	
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if (1 == nIDEvent)
	{
		DWORD begin2 = GetTickCount();
		CString strSystime;
		GetLocalTime(&st);
		strSystime.Format(_T("%02d-%02d_%02d:%02d:%02d:%02d"),st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);	
		//myrealCurrent = 1000*myrealCurrent;//ֱ��ȡ�������������λΪA
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItem(IDC_BUTTON_OPENDIVECE)->GetWindowText(str);
	if (!str.Compare(_T("�򿪵�Դ")))
	{			
		if (!OpenDevice())
		{
			GetDlgItem(IDC_BUTTON_OPENDIVECE)->SetWindowText(_T("�رյ�Դ"));			
		}			
	}
	if (!str.Compare(_T("�رյ�Դ")))
	{
		GetDlgItem(IDC_BUTTON_OPENDIVECE)->SetWindowText(_T("�򿪵�Դ"));
		CloseDevice();
	}
}

void CmeasureDlg::OnBnClickedButtonQuit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
