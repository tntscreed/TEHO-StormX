int g_nCurControlsMode = -1;
int g_ControlsLngFile = -1;

bool g_bToolTipStarted = false;

float 	fHUDRatio 	= 1.0;
int 	iHUDBase 	= iGlobalVar1;
int 	newBase 	= iGlobalVar1;
float   SEACAMPERSP = sti(InterfaceStates.SEACAMPERSP)/100.0; // belamour перспектива МК

#define BI_LOW_RATIO 	0.25
#define BI_HI_RATIO 	4.0
#define BI_DIF_RATIO 	3.75

int CalcHUDBase(float fSlider, float MyScreen)
{
    float fRes = BI_DIF_RATIO * fSlider;
    float curBase = MyScreen / (BI_LOW_RATIO + fRes);

    return makeint(curBase + 0.5);
}

float CalcHUDSlider(float fRatio)
{
    float fRes = fRatio - BI_LOW_RATIO;
    fRes /= BI_DIF_RATIO;

    return fRes;
}

void InitInterface(string iniName)
{ 
	float glowEffect;
	
    fHUDRatio = stf(Render.screen_y) / iGlobalVar1;
    iHUDBase = makeint(iGlobalVar1);
    newBase = iHUDBase;
	
	trace("2 : " + iGlobalVar1);

	g_nCurControlsMode = -1;
	GameInterface.title = "titleOptions";
	g_ControlsLngFile = LanguageOpenFile("ControlsNames.txt");

	if( CheckAttribute(&InterfaceStates,"showGameMenuOnExit") && sti(InterfaceStates.showGameMenuOnExit) == true) {
		LoadGameOptions();
	} else {
		DeleteAttribute( &PlayerProfile, "name" );
		LoadGameOptions();
	}

	IReadVariableBeforeInit();
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	IReadVariableAfterInit();

	SetControlsTabMode(1);

	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);  // boal
	SetEventHandler("eTabControlPress","procTabChange",0);
	SetEventHandler("eventBtnAction","procBtnAction",0);
	SetEventHandler("eventKeyChange","procKeyChange",0);

	SetEventHandler("CheckButtonChange","procCheckBoxChange",0);
	SetEventHandler("eSlideChange","procSlideChange",0);

	SetEventHandler("evntKeyChoose","procKeyChoose",0);
	SetEventHandler("ShowInfo", "ShowInfo", 0);
	SetEventHandler("MouseRClickUP","HideInfo",0);

	SetEventHandler("evFaderFrame","FaderFrame",0);

	aref ar; makearef(ar,objControlsState.key_codes);
	SendMessage(&GameInterface,"lsla",MSG_INTERFACE_MSG_TO_NODE,"KEY_CHOOSER", 0,ar);

	if( sti(Render.full_screen)==0 )
	{
		SetSelectable("GAMMA_SLIDE",false);
		SetSelectable("BRIGHT_SLIDE",false);
		SetSelectable("CONTRAST_SLIDE",false);
	}

	float ftmp1 = -1.0;
	float ftmp2 = -1.0;
	float ftmp3 = -1.0;
	SendMessage(&sound,"leee",MSG_SOUND_GET_MASTER_VOLUME,&ftmp1,&ftmp2,&ftmp3);
	if( ftmp1==-1.0 && ftmp2==-1.0 && ftmp3==-1.0 )
	{
		SetSelectable("MUSIC_SLIDE",false);
		SetSelectable("SOUND_SLIDE",false);
		SetSelectable("DIALOG_SLIDE",false);
	}
	
	// Warship 07.07.09 ќффект свечения
	if(!CheckAttribute(&InterfaceStates, "GlowEffect"))
	{
		InterfaceStates.GlowEffect = 50;
	}
	
	
	glowEffect = sti(InterfaceStates.GlowEffect) / 250.0; // Если делить на 250, то хер, т.к. целое, а если на 250.0 - все гуд
	
	GameInterface.nodes.GLOW_SLIDE.value = glowEffect;
	SendMessage(&GameInterface, "lslf", MSG_INTERFACE_MSG_TO_NODE, "GLOW_SLIDE", 0, glowEffect);
	
	fHUDRatio = stf(Render.screen_y) / iGlobalVar1;
	float sl = CalcHUDSlider(fHUDRatio);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "HUD_SLIDE", 2, makeint(sl * 100.0));
	GameInterface.nodes.hud_slide.value = sl;
	iHUDBase = CalcHUDBase(sl, stf(Render.screen_y));
	SetFormatedText("HUD_DESCRIP_TEXT", Render.screen_y + "  / " + newBase + " : " + fHUDRatio);
	
	// belamour перспектива морской камеры
	if(!CheckAttribute(&InterfaceStates, "SEACAMPERSP"))
	{
		InterfaceStates.SEACAMPERSP = 25; 
	}
	SEACAMPERSP = sti(InterfaceStates.SEACAMPERSP)/100.0;
	GameInterface.nodes.SEACAMPERSP_SLIDE.value = SEACAMPERSP;
	SendMessage(&GameInterface, "lslf", MSG_INTERFACE_MSG_TO_NODE, "SEACAMPERSP_SLIDE", 0, SEACAMPERSP);
	
}

void ProcessCancelExit()
{
	LoadGameOptions();
	ProcessExit();
}

void ProcessOkExit()
{
	// Warship 07.07.09 ќффект свечения
	SetGlowParams(1.0, sti(InterfaceStates.GlowEffect), 2));
	iGlobalVar1 = newBase;
	trace("3 : " + iGlobalVar1);
	SEACAMPERSP = InterfaceStates.SEACAMPERSP; // belamour перспектива МК
	SaveGameOptions();
	ProcessExit();
	Event("eventChangeOption");

	// change sea settings
	SetSeaGridStep(stf(InterfaceStates.SeaDetails));
}

void ProcessExit()
{
	if( CheckAttribute(&InterfaceStates,"showGameMenuOnExit") && sti(InterfaceStates.showGameMenuOnExit) == true) {
		IDoExit(RC_INTERFACE_LAUNCH_GAMEMENU);
		return;
	}

	IDoExit(RC_INTERFACE_OPTIONSCREEN_EXIT);
	if( !CheckAttribute(&InterfaceStates,"InstantExit") || sti(InterfaceStates.InstantExit)==false ) {
		ReturnToMainMenu();
	}
}

void IDoExit(int exitCode)
{
	DelEventHandler("evntKeyChoose","procKeyChoose");
	DelEventHandler("eSlideChange","procSlideChange");
	DelEventHandler("CheckButtonChange","procCheckBoxChange");

	DelEventHandler("eventKeyChange","procKeyChange");
	DelEventHandler("eventBtnAction","procBtnAction");
	DelEventHandler("eTabControlPress","procTabChange");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ShowInfo", "ShowInfo");
	DelEventHandler("MouseRClickUP","HideInfo");
	DelEventHandler("evFaderFrame","FaderFrame");
	DelEventHandler("InterfaceBreak","ProcessCancelExit");  // boal

	LanguageCloseFile( g_ControlsLngFile );

	interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"InstantExit") && sti(InterfaceStates.InstantExit)==true ) {
		EndCancelInterface(true);
	} else {
		EndCancelInterface(false);
	}
	ControlsMakeInvert();
}

void IReadVariableBeforeInit()
{
	GetSoundOptionsData();
	GetMouseOptionsData();
	GetVideoOptionsData();
}

void IReadVariableAfterInit()
{
	GetHerbOptionsData();
	GetControlsStatesData();

	int nShowBattleMode = 0;
	if( CheckAttribute(&InterfaceStates,"ShowBattleMode") ) {
		nShowBattleMode = sti(InterfaceStates.ShowBattleMode);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"BATTLE_MODE_CHECKBOX", 2, 1, nShowBattleMode );

	int nEnabledAutoSaveMode = 1;
	if( CheckAttribute(&InterfaceStates,"EnabledAutoSaveMode") ) {
		nEnabledAutoSaveMode = sti(InterfaceStates.EnabledAutoSaveMode);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"AUTOSAVE_CHECKBOX", 2, 1, nEnabledAutoSaveMode );

	int nEnabledQuestsMarks = 1;
	if( CheckAttribute(&InterfaceStates,"EnabledQuestsMarks") ) {
		nEnabledQuestsMarks = sti(InterfaceStates.EnabledQuestsMarks);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"QUESTMARK_CHECKBOX", 2, 1, nEnabledQuestsMarks );

	int nEnabledShipMarks = 1;
	if( CheckAttribute(&InterfaceStates,"EnabledShipMarks") ) {
		nEnabledShipMarks = sti(InterfaceStates.EnabledShipMarks);
		bDrawBars = nEnabledShipMarks;
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SHIPMARK_CHECKBOX", 2, 1, nEnabledShipMarks );

	int nEnabledSimpleSea = 0;
	if( CheckAttribute(&InterfaceStates,"SimpleSea") ) 
	{
		nEnabledSimpleSea = sti(InterfaceStates.SimpleSea);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SIMPLESEA_CHECKBOX", 2, 1, nEnabledSimpleSea );
	
	// belamour --> 
	int nEnabledDIRECTSAIL = 0;
	if( CheckAttribute(&InterfaceStates,"DIRECTSAIL") ) 
	{
		nEnabledDIRECTSAIL = sti(InterfaceStates.DIRECTSAIL);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"DIRECTSAIL_CHECKBOX", 2, 1, nEnabledDIRECTSAIL );
	
	int nEnabledCREWONDECK = 1;
	if( CheckAttribute(&InterfaceStates,"CREWONDECK") ) 
	{
		nEnabledCREWONDECK = sti(InterfaceStates.CREWONDECK);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CREWONDECK_CHECKBOX", 2, 1, nEnabledCREWONDECK );
	
	int nEnabledCAMERASWING = 1;
	if( CheckAttribute(&InterfaceStates,"CAMERASWING") ) 
	{
		nEnabledCAMERASWING = sti(InterfaceStates.CAMERASWING);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CAMERASWING_CHECKBOX", 2, 1, nEnabledCAMERASWING );
	
	int nEnabledENHANCEDSAILING = 0;
	if( CheckAttribute(&InterfaceStates,"ENHANCEDSAILING") ) 
	{
		nEnabledENHANCEDSAILING = sti(InterfaceStates.ENHANCEDSAILING);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ENHANCEDSAILING_CHECKBOX", 2, 1, nEnabledENHANCEDSAILING );
	
	int nEnabledROTATESKY = 0;
	if( CheckAttribute(&InterfaceStates,"ROTATESKY") ) 
	{
		nEnabledROTATESKY = sti(InterfaceStates.ROTATESKY);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ROTATESKY_CHECKBOX", 2, 1, nEnabledROTATESKY );
	
	int nEnabledDYNAMICLIGHTS = 0;
	if( CheckAttribute(&InterfaceStates,"DYNAMICLIGHTS") ) 
	{
		nEnabledDYNAMICLIGHTS = sti(InterfaceStates.DYNAMICLIGHTS);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"DYNAMICLIGHTS_CHECKBOX", 2, 1, nEnabledDYNAMICLIGHTS );
	
	// <-- belamour
}

void SetControlsTabMode(int nMode)
{
	int nColor1 = argb(255,196,196,196);
	int nColor2 = nColor1;
	int nColor3 = nColor1;
	int nColor4 = nColor1;

	string sPic1 = "TabDeSelected";
	string sPic2 = sPic1;
	string sPic3 = sPic1;
	string sPic4 = sPic1;

	switch( nMode )
	{
	case 1: // море от первого лица
		sPic1 = "TabSelected";
		nColor1 = argb(255,255,255,255);
	break;
	case 2: // режим путешествий на земле
		sPic2 = "TabSelected";
		nColor2 = argb(255,255,255,255);
	break;
	case 3: // море от 3-го лица
		sPic3 = "TabSelected";
		nColor3 = argb(255,255,255,255);
	break;
	case 4: // режим боя на земле
		sPic4 = "TabSelected";
		nColor4 = argb(255,255,255,255);
	break;
	}

	SetNewGroupPicture("TABBTN_SAILING_1ST", "TABS", sPic1);
	SetNewGroupPicture("TABBTN_PRIMARY_LAND", "TABS", sPic2);
	SetNewGroupPicture("TABBTN_SAILING_3RD", "TABS", sPic3);
	SetNewGroupPicture("TABBTN_FIGHT_MODE", "TABS", sPic4);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SAILING_1ST", 8,0,nColor1);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_PRIMARY_LAND", 8,0,nColor2);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_SAILING_3RD", 8,0,nColor3);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"TABSTR_FIGHT_MODE", 8,0,nColor4);

	FillControlsList(nMode);
}

void procTabChange()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();

	if( sNodName == "TABBTN_SAILING_1ST" ) {
		SetControlsTabMode( 1 );
		return;
	}
	if( sNodName == "TABBTN_PRIMARY_LAND" ) {
		SetControlsTabMode( 2 );
		return;
	}
	if( sNodName == "TABBTN_SAILING_3RD" ) {
		SetControlsTabMode( 3 );
		return;
	}
	if( sNodName == "TABBTN_FIGHT_MODE" ) {
		SetControlsTabMode( 4 );
		return;
	}
}

void procBtnAction()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();

	if( sNodName == "BTN_OK" ) {
		if( iComIndex==ACTION_ACTIVATE || iComIndex==ACTION_MOUSECLICK ) {
			ProcessOkExit();
		}
		return;
	}
	if( sNodName == "BTN_CONTROLS_DEFAULT" ) {
		RestoreDefaultKeys();
		return;
	}
}

void procCheckBoxChange()
{
	string sNodName = GetEventData();
	int nBtnIndex = GetEventData();
	int bBtnState = GetEventData();

	if( sNodName == "HERB_CHECKBOX" ) {
		if( bBtnState==true ) {
			switch( nBtnIndex ) {
			case 1: iGrassQuality=3; break;
			case 2: iGrassQuality=2; break;
			case 3: iGrassQuality=1; break;
			case 4: iGrassQuality=0; break;
			}
		}
		return;
	}
	if( sNodName == "ALWAYS_RUN_CHECKBOX" ) 
	{
		{ // always run
			SetAlwaysRun( bBtnState );
		}
	}
	if( sNodName == "INVERT_MOUSE_CHECKBOX" ) 
	{
		{ // invert mouse
			InterfaceStates.InvertCameras = bBtnState;
		}
	}
	if( sNodName == "BATTLE_MODE_CHECKBOX" ) 
	{
		{ // Show battle mode border
			InterfaceStates.ShowBattleMode = bBtnState;
		}
	}
	if( sNodName == "AUTOSAVE_CHECKBOX" ) 
	{
		{ // Show battle mode border
			InterfaceStates.EnabledAutoSaveMode = bBtnState;
		}
	}

	if( sNodName == "QUESTMARK_CHECKBOX" ) 
	{
		{ // Show battle mode border
			InterfaceStates.EnabledQuestsMarks = bBtnState;
		}
	}

	if( sNodName == "SHIPMARK_CHECKBOX" ) 
	{
		{ // Show battle mode border
			InterfaceStates.EnabledShipMarks = bBtnState;
		}
	}

	if( sNodName == "SIMPLESEA_CHECKBOX" ) 
	{
		{ // Show battle mode border
			InterfaceStates.SimpleSea = bBtnState;
		}
	}
	// belamour -->
	if( sNodName == "DIRECTSAIL_CHECKBOX" ) 
	{
		{ 
			InterfaceStates.DIRECTSAIL = bBtnState;
		}
	}
	
	if( sNodName == "CREWONDECK_CHECKBOX" ) 
	{
		{ 
			InterfaceStates.CREWONDECK = bBtnState;
		}
	}
	
	if( sNodName == "CAMERASWING_CHECKBOX" ) 
	{
		{ 
			InterfaceStates.CAMERASWING = bBtnState;
		}
	}
	
	if( sNodName == "ENHANCEDSAILING_CHECKBOX" ) 
	{
		{ 
			InterfaceStates.ENHANCEDSAILING = bBtnState;
		}
	}
	if( sNodName == "ROTATESKY_CHECKBOX" ) 
	{
		{ 
			InterfaceStates.ROTATESKY = bBtnState;
		}
	}
	if( sNodName == "DYNAMICLIGHTS_CHECKBOX" ) 
	{
		{ 
			InterfaceStates.DYNAMICLIGHTS = bBtnState;
		}
	}
	// <-- belamour
}

void procSlideChange()
{
	string sNodeName = GetEventData();
	int nVal = GetEventData();
	float fVal = GetEventData();

	if( sNodeName=="GAMMA_SLIDE" || sNodeName=="BRIGHT_SLIDE" || sNodeName=="CONTRAST_SLIDE" ) {
		ChangeVideoColor();
		return;
	}
	
	// Warship 07.07.09 ќффект свечения
	if(sNodeName == "GLOW_SLIDE")
	{
		InterfaceStates.GlowEffect = fVal*250;
		return;
	}
	
	if( sNodeName == "SEA_DETAILS_SLIDE" ) {
		ChangeSeaDetail();
		return;
	}
	if( sNodeName=="MUSIC_SLIDE" || sNodeName=="SOUND_SLIDE" || sNodeName=="DIALOG_SLIDE" ) {
		ChangeSoundSetting();
		return;
	}
	if( sNodeName == "HUD_SLIDE" ) {
		ChangeHUDDetail();
		return;
	}
	if( sNodeName=="VMOUSE_SENSITIVITY_SLIDE" || sNodeName=="HMOUSE_SENSITIVITY_SLIDE" ) {
		ChangeMouseSensitivity();
	}
	// belamour перспектива МК
	if(sNodeName == "SEACAMPERSP_SLIDE")
	{
		InterfaceStates.SEACAMPERSP = fVal*100;
		return;
	}
}

void ChangeMouseSensitivity()
{
	InterfaceStates.mouse.x_sens = stf(GameInterface.nodes.hmouse_sensitivity_slide.value);
	InterfaceStates.mouse.y_sens = stf(GameInterface.nodes.vmouse_sensitivity_slide.value);
	SetRealMouseSensitivity();
}

void ChangeVideoColor()
{
	float fCurContrast = stf(GameInterface.nodes.contrast_slide.value);
	float fCurGamma = stf(GameInterface.nodes.GAMMA_SLIDE.value);
	float fCurBright = stf(GameInterface.nodes.BRIGHT_SLIDE.value);

	float fContrast = ConvertContrast(fCurContrast,false);
	float fGamma = ConvertGamma(fCurGamma,false);
	float fBright = ConvertBright(fCurBright,false);

	if( !CheckAttribute(&InterfaceStates,"video.contrast") ||
		(stf(InterfaceStates.video.contrast)!=fContrast) ||
		(stf(InterfaceStates.video.gamma)!=fGamma) ||
		(stf(InterfaceStates.video.brightness)!=fBright) ) {
			InterfaceStates.video.contrast = fContrast;
			InterfaceStates.video.gamma = fGamma;
			InterfaceStates.video.brightness = fBright;
			XI_SetColorCorrection(fContrast,fGamma,fBright);
	}
}

void ChangeSeaDetail()
{
	float fCurSeaDetail = stf(GameInterface.nodes.sea_details_slide.value);
	float fSeaDetail = ConvertSeaDetails(fCurSeaDetail,false);
	if( !CheckAttribute(&InterfaceStates,"SeaDetails") ||
		(stf(InterfaceStates.SeaDetails)!=fSeaDetail) ) {
			InterfaceStates.SeaDetails = fSeaDetail;
	}
}

void ChangeSoundSetting()
{
	float fMusic = stf(GameInterface.nodes.music_slide.value);
	float fSound = stf(GameInterface.nodes.sound_slide.value);
	float fDialog = stf(GameInterface.nodes.dialog_slide.value);
	SendMessage(&sound,"lfff", MSG_SOUND_SET_MASTER_VOLUME, fSound,	fMusic,	fDialog);
}

void FillControlsList(int nMode)
{
	int n,qC,idx;
	string groupName;
	aref arGrp, arC;

	if( nMode == g_nCurControlsMode ) {return;}
	g_nCurControlsMode = nMode;
	DeleteAttribute(&GameInterface,"controls_list");
	GameInterface.controls_list.select = 0;

	groupName = GetGroupNameByMode(nMode);
	if( CheckAttribute(&objControlsState,"keygroups."+groupName) ) {
		makearef(arGrp,objControlsState.keygroups.(groupName));
		qC = GetAttributesNum(arGrp);
		idx = 0;
		for( n=0; n<qC; n++ ) {
			arC = GetAttributeN(arGrp,n);
			// --> belamour убрать контролки из опций 
			string sControl = GetAttributeName(arC);
			if(sControl == "Fast_port" || sControl == "Fast_store" || 
			   sControl == "Fast_Shipyard" || sControl == "Fast_tavern" ||
			   sControl == "Fast_townhall" || sControl == "Fast_bank" ||
			   sControl == "Fast_church" || sControl == "Fast_Brothel" ||
			   sControl == "Fast_PortOffice" || sControl == "Fast_prison" ||
			   sControl == "HK_bullet" || sControl == "HK_cartridge" ||
			   sControl == "HK_grapeshot" || sControl == "HK_harpoon" ||
			   sControl == "HK_GunEchin" || sControl == "HK_grenade") continue;
			// <-- belamour
			
			if( false==CheckAttribute(arC,"invisible") || arC.invisible!="1" ) {
			//if( CheckAttribute(arC,"remapping") && arC.remapping=="1" ) {
				if( AddToControlsList( idx, GetAttributeName(arC), GetAttributeValue(arC), CheckAttribute(arC,"remapping") && arC.remapping=="1" ) ) {
					idx++;
				}
			}
		}
	}
	SendMessage( &GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, "CONTROLS_LIST", 0 );
}

bool AddToControlsList(int row, string sControl, string sKey, bool bRemapable)
{
	string rowname = "tr" + (row+1);
	GameInterface.controls_list.(rowname).userdata.remapable = bRemapable;
	GameInterface.controls_list.(rowname).userdata.control = sControl;
	GameInterface.controls_list.(rowname).userdata.key = sKey;
	GameInterface.controls_list.(rowname).td2.str = LanguageConvertString(g_ControlsLngFile,sControl);
	if( GameInterface.controls_list.(rowname).td2.str == "" ) {
		trace("Warning!!! " + sControl + " hav`t translate value");
	}
	if( !bRemapable ) { // выделение контролок которые нельзя поменять
		GameInterface.controls_list.(rowname).td2.color = argb(255,128,128,128);
	}
	if( CheckAttribute(&objControlsState,"key_codes."+sKey+".img") ) {
		GameInterface.controls_list.(rowname).td1.fontidx = 0;
		GameInterface.controls_list.(rowname).td1.textoffset = "2,-1";
		GameInterface.controls_list.(rowname).td1.scale = 0.5;
		GameInterface.controls_list.(rowname).td1.str = objControlsState.key_codes.(sKey).img;
	} else {
		GameInterface.controls_list.(rowname).td1.str = "";
	}
	return true;
}

string GetGroupNameByMode(int nMode)
{
	switch( nMode ) 
	{
		case 1: return "Sailing1Pers"; break;
		case 2: return "PrimaryLand"; break;
		case 3: return "Sailing3Pers"; break;
		case 4: return "FightModeControls"; break;
	}
	return "unknown";
}

void GetSoundOptionsData()
{   // belamour побережем уши )))
	float fCurMusic = 0.1;
	float fCurSound = 0.1;
	float fCurDialog = 0.1;
	SendMessage(&sound,"leee",MSG_SOUND_GET_MASTER_VOLUME,&fCurSound,&fCurMusic,&fCurDialog);
	GameInterface.nodes.music_slide.value = fCurMusic;
	GameInterface.nodes.sound_slide.value = fCurSound;
	GameInterface.nodes.dialog_slide.value = fCurDialog;
}

void GetMouseOptionsData()
{
	float fCurXSens = 0.5;
	float fCurYSens = 0.5;
	if( CheckAttribute(&InterfaceStates,"mouse.x_sens") ) {fCurXSens=stf(InterfaceStates.mouse.x_sens);}
	if( CheckAttribute(&InterfaceStates,"mouse.y_sens") ) {fCurYSens=stf(InterfaceStates.mouse.y_sens);}
	if(fCurXSens<0.0) fCurXSens = 0.0;
	if(fCurXSens>1.0) fCurXSens = 1.0;
	if(fCurYSens<0.0) fCurYSens = 0.0;
	if(fCurYSens>1.0) fCurYSens = 1.0;
	GameInterface.nodes.hmouse_sensitivity_slide.value = fCurXSens;
	GameInterface.nodes.vmouse_sensitivity_slide.value = fCurYSens;
}

void GetVideoOptionsData()
{
	float fC = 1.0;
	float fG = 1.0;
	float fB = 0.0;
	float fD = 1.0;

	if( CheckAttribute(&InterfaceStates,"video.contrast") ) {
		fC = stf(InterfaceStates.video.contrast);
	}
	if( CheckAttribute(&InterfaceStates,"video.gamma") ) {
		fG = stf(InterfaceStates.video.gamma);
	}
	if( CheckAttribute(&InterfaceStates,"video.brightness") ) {
		fB = stf(InterfaceStates.video.brightness);
	}

	if( CheckAttribute(&InterfaceStates,"SeaDetails") ) {
		fD = stf(InterfaceStates.SeaDetails);
	}

	ISetColorCorrection(fC, fG, fB, fD);
}

void ISetColorCorrection(float fContrast, float fGamma, float fBright, float fSeaDetails)
{
	float fCurContrast = ConvertContrast(fContrast,true);
	float fCurGamma = ConvertGamma(fGamma,true);
	float fCurBright = ConvertBright(fBright,true);
	float fCurSeaDetails = ConvertSeaDetails(fSeaDetails, true);

	if(fCurContrast>1.0) fCurContrast = 1.0;
	if(fCurContrast<0.0) fCurContrast = 0.0;
	if(fCurGamma>1.0) fCurGamma = 1.0;
	if(fCurGamma<0.0) fCurGamma = 0.0;
	if(fCurBright>1.0) fCurBright = 1.0;
	if(fCurBright<0.0) fCurBright = 0.0;
	if(fCurSeaDetails<0.0) fCurSeaDetails = 0.0;
	if(fCurSeaDetails>1.0) fCurSeaDetails = 1.0;

	GameInterface.nodes.CONTRAST_SLIDE.value = fCurContrast;
	GameInterface.nodes.GAMMA_SLIDE.value = fCurGamma;
	GameInterface.nodes.BRIGHT_SLIDE.value = fCurBright;
	GameInterface.nodes.SEA_DETAILS_SLIDE.value = fCurSeaDetails;

	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"CONTRAST_SLIDE", 0,fCurContrast);
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"GAMMA_SLIDE", 0,fCurGamma);
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"BRIGHT_SLIDE", 0,fCurBright);

	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"SEA_DETAILS_SLIDE", 0, fCurSeaDetails);

	XI_SetColorCorrection(fContrast,fGamma,fBright);
	//Set sea detail
}

float ConvertContrast(float fContrast, bool Real2Slider)
{ // контрастность от 0.75 до 1.25
	if(Real2Slider) {
		return fContrast*2.0-1.5;
	}
	return fContrast*0.5+0.75;
}

float ConvertGamma(float fGamma, bool Real2Slider)
{ // гамма от 0.5 до 2.0
	if(Real2Slider)
	{
		if(fGamma<=1.0) {return fGamma-0.5;}
		return fGamma*0.5;
	}
	if(fGamma<=0.5) {return fGamma+0.5;}
	return fGamma*2.0;
}

float ConvertBright(float fBright, bool Real2Slider)
{
	if(Real2Slider) {
		return (fBright+50.0)/100.0;
	}
	return fBright*100-50;
}

float ConvertSeaDetails(float fDetails, bool Real2Slider)
{
	return fDetails;
}

void GetHerbOptionsData()
{
	int nSelBtn = 0;
	switch( iGrassQuality ) 
	{
		case 0: nSelBtn=4; break;
		case 1: nSelBtn=3; break;
		case 2: nSelBtn=2; break;
		case 3: nSelBtn=1; break;
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"HERB_CHECKBOX", 2, nSelBtn, true );
}

void GetControlsStatesData()
{
	int nAlwaysRun = 0;
	if( CheckAttribute(&InterfaceStates,"alwaysrun") ) {
		nAlwaysRun = sti(InterfaceStates.alwaysrun);
	}
	int nInvertCam = 0;
	if( CheckAttribute(&InterfaceStates,"InvertCameras") ) {
		nInvertCam = sti(InterfaceStates.InvertCameras);
	}
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ALWAYS_RUN_CHECKBOX", 2, 1, nAlwaysRun );
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INVERT_MOUSE_CHECKBOX", 2, 1, nInvertCam );
}

void SetAlwaysRun(bool bRun)
{
	InterfaceStates.alwaysrun = bRun;
}

void procKeyChange()
{
	//FillControlsList();
	string srow = "tr" + GameInterface.controls_list.select;
	if( !CheckAttribute(&GameInterface,"controls_list."+srow) ) {return;}
	if( sti(GameInterface.controls_list.(srow).userdata.remapable)!=1 ) {return;}
	ChooseOtherControl();
}

void ChooseOtherControl()
{
	XI_WindowDisable("MAIN_WINDOW",true);
	XI_WindowShow("CHANGEKEY_WINDOW",true);
	SetCurrentNode("KEY_CHOOSER");
	string srow = "tr" + GameInterface.controls_list.select;
	SetFormatedText("CHANGEKEY_TEXT", XI_ConvertString("Press any key"));
	AddLineToFormatedText("CHANGEKEY_TEXT", " ");
	AddLineToFormatedText("CHANGEKEY_TEXT", GameInterface.controls_list.(srow).td2.str);
	AddLineToFormatedText("CHANGEKEY_TEXT", " ");
	AddLineToFormatedText("CHANGEKEY_TEXT", XI_ConvertString("KeyAlreadyUsed"));
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 8, 0, argb(255,255,128,128) );
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 8, 4, argb(0,255,64,64) );
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 5);
}

int glob_retVal;
ref procKeyChoose()
{
	int keyIdx = GetEventData();
	int stickUp = GetEventData();

	glob_retVal = false;

	if (keyIdx == 7) {
		ReturnFromReassign();
		glob_retVal = true;
		return &glob_retVal;
	}

	if( DoMapToOtherKey(keyIdx,stickUp) )
	{
		ReturnFromReassign();
		glob_retVal = true;
	}

	return &glob_retVal;
}

void ReturnFromReassign()
{
	XI_WindowShow("CHANGEKEY_WINDOW",false);
	XI_WindowDisable("MAIN_WINDOW",false);
	SetCurrentNode("CONTROLS_LIST");
}

bool DoMapToOtherKey(int keyIdx,int stickUp)
{
	string srow = "tr" + GameInterface.controls_list.select;
	string groupName = GetGroupNameByMode( g_nCurControlsMode );
	string sControl = GameInterface.controls_list.(srow).userdata.control;
	string sKey = GameInterface.controls_list.(srow).userdata.key;


	aref arControlGroup;
	aref arKeyRoot,arKey;
	string tmpstr;
	int keyCode;

	if( stickUp )
	{
		//SetStickNotAvailable();
		return false;
	}

	makearef(arControlGroup,objControlsState.keygroups.(groupName));
	makearef(arKeyRoot,objControlsState.key_codes);
	arKey = GetAttributeN(arKeyRoot,keyIdx);
	keyCode = sti(GetAttributeValue(arKey));
    string sCon = GetAttributeValue(arKey);
	// check for not allowed keys
	if( keyCode==sti(objControlsState.key_codes.VK_F1) ||
		keyCode==sti(objControlsState.key_codes.VK_F2) ||
		keyCode==sti(objControlsState.key_codes.VK_F3) ||
		keyCode==sti(objControlsState.key_codes.VK_F4) ||
		keyCode==sti(objControlsState.key_codes.VK_F5) ||
		keyCode==sti(objControlsState.key_codes.VK_F6) ||
		keyCode==sti(objControlsState.key_codes.VK_F7) ||
		keyCode==sti(objControlsState.key_codes.VK_F8) ||
		keyCode==sti(objControlsState.key_codes.VK_F9) ||
		keyCode==sti(objControlsState.key_codes.VK_F10) || // belamour все эти кнопки в исключение
		keyCode==sti(objControlsState.key_codes.VK_F11) ||
		keyCode==sti(objControlsState.key_codes.VK_F12)  )
	{
		return false;
	}
	
	if( CheckAttribute(arKey,"stick") && sti(arKey.stick)==true ) return false;

	if( KeyAlreadyUsed(groupName, sControl, GetAttributeName(arKey)) )
	{
		SetKeyChooseWarning( XI_ConvertString("KeyAlreadyUsed") );
		return false;
	}

	tmpstr = arControlGroup.(sControl);
	if( CheckAttribute(arKeyRoot,tmpstr+".stick") && sti(arKeyRoot.(tmpstr).stick)==true ) return false;

	int state = 0;
	if(CheckAttribute(arControlGroup,sControl+".state"))
	{	state = sti(arControlGroup.(sControl).state);	}

	CI_CreateAndSetControls( groupName,sControl,keyCode, state, true );
	GameInterface.controls_list.(srow).userdata.key = arKey;
	GameInterface.controls_list.(srow).td1.str = arKey.img;
	SendMessage( &GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, "CONTROLS_LIST", 0 );
	return true;
}

void RestoreDefaultKeys()
{
	SetMouseToDefault();
	ControlsInit(GetTargetPlatform(),false);

	int nMode = g_nCurControlsMode;
	g_nCurControlsMode = -1;
	FillControlsList(nMode);
}

void SetMouseToDefault()
{
	InterfaceStates.InvertCameras = false;
	InterfaceStates.mouse.x_sens = 0.5;
	InterfaceStates.mouse.y_sens = 0.5;

	SetRealMouseSensitivity();
	SetAlwaysRun(true);

	GetControlsStatesData();
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"VMOUSE_SENSITIVITY_SLIDE", 0,stf(InterfaceStates.mouse.y_sens));
	SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"HMOUSE_SENSITIVITY_SLIDE", 0,stf(InterfaceStates.mouse.x_sens));
}

void ShowInfo()
{
	g_bToolTipStarted = true;
	string sHeader = "TEST";
	string sNode = GetCurrentNode();

	string sText1, sText2, sText3, sPicture, sGroup, sGroupPicture;
	sPicture = "none";
	sGroup = "none";
	sGroupPicture = "none";

	switch (sNode)
	{
		case "GAMMA_SLIDE":
			sHeader = XI_ConvertString("gamma");
			sText1 = XI_ConvertString("gamma_descr");
			sText3 = XI_ConvertString("FullScreenOnly");
		break;
		case "BRIGHT_SLIDE":
			sHeader = XI_ConvertString("Brightness");
			sText1 = XI_ConvertString("brightness_descr");
			sText3 = XI_ConvertString("FullScreenOnly");
		break;
		case "CONTRAST_SLIDE":
			sHeader = XI_ConvertString("Contrast");
			sText1 = XI_ConvertString("Contrast_descr");
			sText3 = XI_ConvertString("FullScreenOnly");
		break;
		
		case "GLOW_SLIDE":
			sHeader = XI_ConvertString("Glow");
			sText1 = XI_ConvertString("Glow_descr");
			sText2 = XI_ConvertString("PostProcessOnly");
		break;
		
		case "SEA_DETAILS_SLIDE":
			sHeader = XI_ConvertString("Sea Detail");
			sText1 = XI_ConvertString("Sea Detail_descr");
			sText2 = XI_ConvertString("ItCanRedusePerfomance");
			sText3 = XI_ConvertString("NeedToExitFromSea");
		break;
		
		case "HERB_CHECKBOX":
			sHeader = XI_ConvertString("Herb Quantity");
			sText1 = XI_ConvertString("Herb Quantity_descr");
			sText2 = XI_ConvertString("ItCanRedusePerfomance");
			//sText3 = XI_ConvertString("NeedToExitFromSea");
		break;

		case "MUSIC_SLIDE":
			sHeader = XI_ConvertString("Music Volume");
			sText1 = XI_ConvertString("Music Volume_descr");
		break;

		case "SOUND_SLIDE":
			sHeader = XI_ConvertString("Sound Volume");
			sText1 = XI_ConvertString("Sound Volume_descr");
		break;

		case "DIALOG_SLIDE":
			sHeader = XI_ConvertString("Dialog Volume");
			sText1 = XI_ConvertString("Dialog Volume_descr");
		break;

		case "ALWAYS_RUN_CHECKBOX":
			sHeader = XI_ConvertString("Always Run");
			sText1 = XI_ConvertString("Always Run_descr");
		break;

		case "INVERT_MOUSE_CHECKBOX":
			sHeader = XI_ConvertString("Invert Vertical Mouse Control");
			sText1 = XI_ConvertString("Invert Vertical Mouse Control_descr");
		break;

		case "VMOUSE_SENSITIVITY_SLIDE":
			sHeader = XI_ConvertString("Vertical Mouse Sensitivity");
			sText1 = XI_ConvertString("Vertical Mouse Sensitivity_descr");
		break;

		case "HMOUSE_SENSITIVITY_SLIDE":
			sHeader = XI_ConvertString("Horizontal Mouse Sensitivity");
			sText1 = XI_ConvertString("Horizontal Mouse Sensitivity_descr");
		break;

		case "BATTLE_MODE_CHECKBOX":
			sHeader = XI_ConvertString("Show battle mode");
			sText1 = XI_ConvertString("Show battle mode_descr");
		break;

		case "AUTOSAVE_CHECKBOX":
			sHeader = XI_ConvertString("AutoSave Mode");
			sText1 = XI_ConvertString("AutoSave Mode_descr");
		break;

		case "QUESTMARK_CHECKBOX":
			sHeader = XI_ConvertString("QuestMark Mode");
			sText1 = XI_ConvertString("QuestMark Mode_descr");
		break;

		case "SHIPMARK_CHECKBOX":
			sHeader = XI_ConvertString("ShipMark Mode");
			sText1 = XI_ConvertString("ShipMark Mode_descr");
		break;

		case "SIMPLESEA_CHECKBOX":
			sHeader = XI_ConvertString("SimpleSea Mode");
			sText1 = XI_ConvertString("SimpleSea Mode_descr");
		break;
		// belamour
		case "DIRECTSAIL_CHECKBOX":
			sHeader = XI_ConvertString("DIRECTSAIL Mode");
			sText1 = XI_ConvertString("DIRECTSAIL Mode_descr");
		break;
		
		case "CAMERASWING_CHECKBOX":
			sHeader = XI_ConvertString("CAMERASWING Mode");
			sText1 = XI_ConvertString("CAMERASWING Mode_descr");
		break;
		
		case "ENHANCEDSAILING_CHECKBOX":
			sHeader = XI_ConvertString("ENHANCEDSAILING Mode");
			sText1 = XI_ConvertString("ENHANCEDSAILING Mode_descr");
		break;
		
		case "CREWONDECK_CHECKBOX":
			sHeader = XI_ConvertString("CREWONDECK Mode");
			sText1 = XI_ConvertString("CREWONDECK Mode_descr");
		break;
		
		case "ROTATESKY_CHECKBOX":
			sHeader = XI_ConvertString("ROTATESKY Mode");
			sText1 = XI_ConvertString("ROTATESKY Mode_descr");
			sText2 = XI_ConvertString("ROTATESKYatt Mode_descr");
		break;
		
		case "DYNAMICLIGHTS_CHECKBOX":
			sHeader = XI_ConvertString("DYNAMICLIGHTS Mode");
			sText1 = XI_ConvertString("DYNAMICLIGHTS Mode_descr");
			sText2 = XI_ConvertString("DYNAMICLIGHTSatt Mode_descr");
		break;
		
		case "SEACAMPERSP_SLIDE":
			sHeader = XI_ConvertString("SEACAMPERSP_SLIDE Mode");
			sText1 = XI_ConvertString("SEACAMPERSP_SLIDE Mode_descr");
			sText2 = XI_ConvertString("NeedToExitFromSea");
		break;
	}

	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,255,255,255), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);
}

void HideInfo()
{
	if( g_bToolTipStarted ) {
		g_bToolTipStarted = false;
		CloseTooltip();
		SetCurrentNode("OK_BUTTON");
	}
}

bool KeyAlreadyUsed(string sGrpName, string sControl, string sKey)
{
	if(!CheckAttribute(&objControlsState,"keygroups."+sGrpName+"."+sControl) ) {return false;} 
	if( objControlsState.keygroups.(sGrpName).(sControl) == sKey ) {return false;} 
	/// belamour контролки без группы не переназначаются
	if(sKey == "KEY_N" || sKey == "VK_ADD" || sKey == "VK_SUBTRACT"
	|| sKey == "KEY_R" || sKey == "VK_A_PLUS" || sKey == "VK_A_MINUS") return true; 
	
	bool bAlreadyUsed = false;
	int n,q, i,grp;
	aref arGrp,arCntrl, arGrpList;

	// проверка на совпадение в той же группе
	makearef(arGrp,objControlsState.keygroups.(sGrpName));
	q = GetAttributesNum(arGrp);
	for(n=0; n<q; n++)
	{
		arCntrl = GetAttributeN(arGrp,n);			   
		if( GetAttributeValue(arCntrl) == sKey ) {
			// belamour
			if(CheckAttribute(&objControlsState,"keygroups.AltPressedGroup"+"."+GetAttributeName(arCntrl))) continue;
			bAlreadyUsed = true;
			break;
		}
	}

	if( bAlreadyUsed ) {return bAlreadyUsed;}

	// найдем группу в которой эта контролка также отображается
	makearef(arGrpList, objControlsState.keygroups);
	grp = GetAttributesNum(arGrpList);
	for( i=0; i<grp; i++ )
	{
		arGrp = GetAttributeN(arGrpList,i);
		if( !CheckAttribute(arGrp,sControl) ) {continue;}
		
		q = GetAttributesNum(arGrp);
		for(n=0; n<q; n++)
		{
			arCntrl = GetAttributeN(arGrp,n);					  
			if( GetAttributeValue(arCntrl) == sKey ) {
				// belamour
				if(CheckAttribute(&objControlsState,"keygroups.AltPressedGroup"+"."+GetAttributeName(arCntrl))) continue;
				bAlreadyUsed = true;
				break;
			}
		}
		if( bAlreadyUsed ) {break;}
	}

	return bAlreadyUsed;
}

void SetKeyChooseWarning( string sWarningText )
{
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 10, 4, &sWarningText );
	SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 5 );
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 8, 4, argb(255,255,64,64) );
	PostEvent("evFaderFrame",700,"lll",500,0,50);
}

void FaderFrame()
{
	int nTotalTime = GetEventData();
	int nCurTime = GetEventData();
	int nDeltaTime = GetEventData();

	nCurTime = nCurTime + nDeltaTime;
	if( nCurTime>nTotalTime ) {nCurTime=nTotalTime;}

	int nAlpha = 255*(nTotalTime-nCurTime) / nTotalTime;
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHANGEKEY_TEXT", 8, 4, argb(nAlpha,255,64,64) );

	if( nCurTime<nTotalTime ) {
		PostEvent("evFaderFrame",nDeltaTime,"lll",nTotalTime,nCurTime,nDeltaTime);
	}
}

void ChangeHUDDetail()
{
    float sl = stf(GameInterface.nodes.hud_slide.value);
	newBase = CalcHUDBase(sl, stf(Render.screen_y));
	if( newBase != iHUDBase) 
	{
        fHUDRatio = stf(Render.screen_y) / newBase;
		SetFormatedText("HUD_DESCRIP_TEXT", Render.screen_y + "  / " + newBase + " : " + fHUDRatio);
	}
}

