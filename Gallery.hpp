//---------------------------------------------------------
//  Name: Gallery
//  Current Ver: 2.4.0
//  Platform: HP Prime
//  Dev: Manuel Velez
//  Licence: GPL
//---------------------------------------------------------

//---------------------------------------------------------
//---------------- Gallery DEFS Start Here ----------------
//---------------------------------------------------------


//----Main Declaration----
EXPORT Gallery() 
BEGIN
END;


//----Directives----
#pragma mode(separator(.,;) integer(h32))


//----Variables----
LOCAL FILE_LIST;
LOCAL CURRENT_FILE:=1;
LOCAL REVERSE_ORDER:=0;
LOCAL IMAGE_INFO:=0;
LOCAL PRESSED_KEY;
LOCAL STOP_GALLERY:=0;
LOCAL IMAGE_WIDTH,IMAGE_HEIGHT;
LOCAL X_AXIS_SHIFT,Y_AXIS_SHIFT;
LOCAL SCROLL_SPEED:=2;
LOCAL EXPAND_IMAGE_TO_FULL_SCREEN:=0;
LOCAL DARK_MODE:=0;
LOCAL X_AXIS_TOUCH,Y_AXIS_TOUCH;
LOCAL X_AXIS_TOUCH_SHIFT,Y_AXIS_TOUCH_SHIFT;
LOCAL TZ_1,TZ_2,TZ_3,TZ_4;
LOCAL TZ_ZOOM_SENSITIVITY:=0.025;


//----Methods----
Open_File();
Delete_File();
Delete_All_Files();
Read_Keyboard();
Invert_Colors();
Center_Fit();
Scroll_Down();
Scroll_Up();
Scroll_Left();
Scroll_Right();
Zoom_In();
Zoom_Out();
Touch_Gestures();
Touch_Zoom();
Clear_Background();

//----Menus----
Delete_Menu();


//Removing those functions from template app
//to prevent unexpected behavior
//----Redefinitions----
Symb() BEGIN END;
SymbSetScroll_Down() BEGIN END;
PlotSetScroll_Down() BEGIN END;
Plot() BEGIN END;
Num() BEGIN END;
NumSetScroll_Down() BEGIN END;


//------------------------------------------------------------
//---------------- Gallery DIALOGS Start Here ----------------
//------------------------------------------------------------


//----Help Dialog----
//This dialog must be named START
//Because it's the main screen
VIEW "Help",START()
BEGIN 
  IF POS(AFiles(),"icon.png")==0
  THEN
    PRINT();
    PRINT("Sorry, the Gallery APP is corrupted and cannot be started. Please reinstall it from the original source.");
    STARTVIEW(-4,1);
    KILL;
  ELSE
    FILE_LIST:=sort(difference({"icon.png"},AFiles()));
    IF REVERSE_ORDER==1
    THEN
      FILE_LIST:=reverse(FILE_LIST);
    END;
    STARTVIEW(6,1);
  END;
END;


//----Open File Dialog----
VIEW "Open Files",Open_File_Dialog()
BEGIN
  IF length(FILE_LIST)>0
  THEN
    CHOOSE(CURRENT_FILE, "Select File to Open", FILE_LIST);
    Open_File();
  ELSE
    MSGBOX("There are no files to open.");
    START();
  END;
END;


//----Open Page Dialog----
VIEW "Open Page Number",Go_To_Dialog()
BEGIN
  INPUT(CURRENT_FILE, "Go to page/file number","Page: ","Enter page or file number to open",1);
  Open_File();
END;


//----Delete File Dialog----
VIEW "Delete Files",DELETE()
BEGIN 
  IF length(FILE_LIST)>0
  THEN
    Delete_Menu();
  ELSE
    MSGBOX("There are no files to delete.");
    START();
  END;
END;

//----Options Dialog----
VIEW "Options",OPTIONS()
BEGIN
INPUT( {{IMAGE_INFO,1}, {DARK_MODE,1}, {EXPAND_IMAGE_TO_FULL_SCREEN,1}, {REVERSE_ORDER,1}, {SCROLL_SPEED,[1]}, {TZ_ZOOM_SENSITIVITY,[0]}}, "Gallery Options", {"Image Info","Invert Colors","Fit-To-Screen","Reverse Order","Scroll Speed", "Touch Zoom Sensitivity"},{"Show OSD with the image name and number","Invert colors to negative (Dark Mode)","Fit Image to Full Width or Height automatically","Reverse order in Open Files list","Scroll Speed when using keyboard (Default 2)","Touch Zoom Sensitivity value (Default 0.025)"});
START();
RETURN;
END;


//----About Dialog----
VIEW "About Gallery",ABOUT()
BEGIN 
  MSGBOX("Gallery: The HP Prime PNG & JPG Image Viewer! V2.4.0 Manuel Andrés Vélez, 2023. Write me at: mandresve@hotmail.com");
END;


//----------------------------------------------------------
//---------------- Gallery MENUS Start Here ----------------
//----------------------------------------------------------


//----Delete Menu----
Delete_Menu()
BEGIN 
  LOCAL USER_CHOICE:=0;
  CHOOSE(USER_CHOICE, "Delete Files?", "Select File", "All Files", "Cancel");

  IF USER_CHOICE==1 
  THEN
    Delete_File();
    RETURN;
  END;

  IF USER_CHOICE==2 
  THEN
    Delete_All_Files();
    RETURN;
  END;

  IF USER_CHOICE==3
  THEN 
    START();
    RETURN;
  END;
END;


//-----------------------------------------------------------------
//---------------- Gallery MGMT METHODS Start Here ----------------
//-----------------------------------------------------------------


//----Open File----
Open_File()
BEGIN
  STOP_GALLERY:=0;
  G1:=AFiles(FILE_LIST[CURRENT_FILE]);
  BLIT_P(G0,G1);
  Center_Fit();

  IF DARK_MODE==1
  THEN
    Invert_Colors();
  END;

  Read_Keyboard();
END;


//----Read Keyboard----
Read_Keyboard() 
BEGIN
  
  //Poll keys on REPEAT loop
  REPEAT
    PRESSED_KEY:=GETKEY;

    //Next Image [DPad Right]
    IF PRESSED_KEY==8
    THEN 
      CURRENT_FILE:=CURRENT_FILE+1;

      IF CURRENT_FILE>=length(FILE_LIST)
      THEN
        CURRENT_FILE:=length(FILE_LIST);
      END;

      Open_File();
    END;

    //Previous Image [DPad Left]
    IF PRESSED_KEY==7
    THEN
      CURRENT_FILE:=CURRENT_FILE-1;

      IF CURRENT_FILE<=0
      THEN
        CURRENT_FILE:=1;
      END;

      Open_File();
    END;


    //Blackout [Backspace]
    IF PRESSED_KEY==19
    THEN 
      RECT_P(0,0,#000000);
      WAIT(.50);
      REPEAT 
        FREEZE;
      UNTIL GETKEY==19;
      WAIT(.1);
      BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
    END;

    //Invert Colors [Sign (+/-)]
    IF PRESSED_KEY==27
    THEN
      IF DARK_MODE==0
      THEN
        DARK_MODE:=1;
      ELSE
        DARK_MODE:=0;
      END;
      Invert_Colors();
    END;

    //Zoom In [Plus]
    IF PRESSED_KEY==50
    THEN
      Zoom_In();
    END;

    //Zoom Out [Minus]
    IF PRESSED_KEY==45
    THEN 
      Zoom_Out();
    END;

    //Scroll Right [6]
    IF PRESSED_KEY==39
    THEN
      Scroll_Right();
    END;

    //Scroll Left [4]
    IF PRESSED_KEY==37
    THEN
      Scroll_Left();
    END;

    //Scroll Down [2]
    IF PRESSED_KEY==43
    THEN
      Scroll_Down();
    END;

    //Scroll Up [8]
    IF PRESSED_KEY==33
    THEN 
      Scroll_Up();
    END;

    //Center & Fit Image [5]
    IF PRESSED_KEY==38
    THEN
      IF EXPAND_IMAGE_TO_FULL_SCREEN==0
      THEN
        EXPAND_IMAGE_TO_FULL_SCREEN:=1;
      ELSE
        EXPAND_IMAGE_TO_FULL_SCREEN:=0;
      END;
      Center_Fit();
    END;

    //Hide App [Home]
    IF PRESSED_KEY==5
    THEN
      STARTVIEW(-1,1);
    END;

    //Poll Touch Gestures
    Touch_Gestures();

    //Exit REPEAT loop when pressed [ON] or [ESC]
    IF PRESSED_KEY==46
    THEN
      STOP_GALLERY:=1;
    END;

    IF PRESSED_KEY==4
    THEN
      STOP_GALLERY:=1;
    END;

  UNTIL STOP_GALLERY==1;
  Open_File_Dialog();
  START();
  RETURN;

END;


//----Delete File----
Delete_File()
BEGIN 
  CHOOSE(CURRENT_FILE, "Select File to Delete...", FILE_LIST);
  LOCAL USER_CHOICE:=0;
  CHOOSE(USER_CHOICE, "Are You Sure to Delete?", "Yes", "No");

    IF USER_CHOICE==1 
    THEN
      DelAFiles(FILE_LIST[CURRENT_FILE]);
      MSGBOX("File Deleted.");
      START();
      RETURN;
    END;

    IF USER_CHOICE==2
    THEN
      MSGBOX("No Files were Deleted.");
    START();
    RETURN;
    END;

END;


//----Delete All Files----
Delete_All_Files()
BEGIN
  LOCAL USER_CHOICE:=0;
  CHOOSE(USER_CHOICE, "Delete All Files?", "Yes", "No");

  IF USER_CHOICE==1
  THEN
    LOCAL LAST_ELEMENT:=length(AFiles())-1;
    FOR I FROM 1 TO LAST_ELEMENT STEP 1
    DO 
      IF (AFiles(1)<>"icon.png")
      THEN
        DelAFiles(AFiles(1));
      ELSE
        DelAFiles(AFiles(2));
      END;
    END;
    MSGBOX("All Files were Deleted!");
    START();
    RETURN;
  ELSE
    MSGBOX("No Files Deleted.");
    START();
    RETURN;
  END;
END;


//------------------------------------------------------------------
//---------------- Gallery IMAGE METHODS Start Here ----------------
//------------------------------------------------------------------


//----Invert Colors----
Invert_Colors()
BEGIN
  INVERT(G1);
  BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
END;


//----Center & Fit Image to Screen----
Center_Fit() 
BEGIN
  X_AXIS_SHIFT:=0;
  Y_AXIS_SHIFT:=0;
  IMAGE_WIDTH:=GROBW_P(G1);
  IMAGE_HEIGHT:=GROBH_P(G1);

  IF EXPAND_IMAGE_TO_FULL_SCREEN==0
  THEN

    IF IMAGE_WIDTH>320 
    THEN 
      REPEAT
        IMAGE_WIDTH:=IMAGE_WIDTH-(IMAGE_WIDTH/100);
        IMAGE_HEIGHT:=IMAGE_HEIGHT-(IMAGE_HEIGHT/100);
      UNTIL IMAGE_WIDTH<320;
    END;

    IF IMAGE_HEIGHT>240
    THEN 
      REPEAT
        IMAGE_WIDTH:=IMAGE_WIDTH-(IMAGE_WIDTH/100);
        IMAGE_HEIGHT:=IMAGE_HEIGHT-(IMAGE_HEIGHT/100);
      UNTIL IMAGE_HEIGHT<240;
    END;

  ELSE

    IF IMAGE_WIDTH>320 
    THEN 
      REPEAT
        IMAGE_WIDTH:=IMAGE_WIDTH-(IMAGE_WIDTH/100);
        IMAGE_HEIGHT:=IMAGE_HEIGHT-(IMAGE_HEIGHT/100);
      UNTIL IMAGE_WIDTH<320;
    END;

    IF IMAGE_HEIGHT<240
    THEN 
      REPEAT
        IMAGE_WIDTH:=IMAGE_WIDTH+(IMAGE_WIDTH/100);
        IMAGE_HEIGHT:=IMAGE_HEIGHT+(IMAGE_HEIGHT/100);
      UNTIL IMAGE_HEIGHT>240;
    END;

  END;

  //Draw Image on Screen
  RECT_P(0,0,#000000);
  BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);

  //Print Image Info
  IF IMAGE_INFO==1
  THEN
    TEXTOUT_P(FILE_LIST[CURRENT_FILE],5,5,1,#FFFFFF,300,#314360);
    TEXTOUT_P(CURRENT_FILE,295,5,1,#FFFFFF,300,#314360);
  END;

  RETURN;
END;


//----Scroll Left----
Scroll_Left() 
BEGIN
  X_AXIS_SHIFT:=X_AXIS_SHIFT+SCROLL_SPEED;
  Clear_Background();
  BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
  WAIT(.1);
  WHILE ISKEYDOWN(37) 
  DO 
    X_AXIS_SHIFT:=X_AXIS_SHIFT+SCROLL_SPEED;
    Clear_Background();
    BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
    WAIT(.02);
  END;
END;


//----Scroll Right----
Scroll_Right() 
BEGIN
  X_AXIS_SHIFT:=X_AXIS_SHIFT-SCROLL_SPEED;
  Clear_Background();
  BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
  WAIT(.05);
  WHILE ISKEYDOWN(39) 
  DO
    X_AXIS_SHIFT:=X_AXIS_SHIFT-SCROLL_SPEED;
    Clear_Background();
    BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
    WAIT(.02);
  END;
END;


//----Scroll Up----
Scroll_Up()
BEGIN
  Y_AXIS_SHIFT:=Y_AXIS_SHIFT+SCROLL_SPEED;
  Clear_Background();
  BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
  WHILE ISKEYDOWN(33)
  DO
    Y_AXIS_SHIFT:=Y_AXIS_SHIFT+SCROLL_SPEED;
    Clear_Background();
    BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
    WAIT(.02);
    END;
END;


//----Scroll Down----
Scroll_Down()
BEGIN
  Y_AXIS_SHIFT:=Y_AXIS_SHIFT-SCROLL_SPEED;
  Clear_Background();
  BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
  WHILE ISKEYDOWN(43)
  DO
    Y_AXIS_SHIFT:=Y_AXIS_SHIFT-SCROLL_SPEED;
    Clear_Background();
    BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
    WAIT(.02);
    END;
END;


//----Zoom In----
Zoom_In()
BEGIN
  IMAGE_WIDTH:=IMAGE_WIDTH+(IMAGE_WIDTH/100);
  IMAGE_HEIGHT:=IMAGE_HEIGHT+(IMAGE_HEIGHT/100);
  BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
  WHILE ISKEYDOWN(50) 
  DO
    IMAGE_WIDTH:=IMAGE_WIDTH+(IMAGE_WIDTH/100);
    IMAGE_HEIGHT:=IMAGE_HEIGHT+(IMAGE_HEIGHT/100);
    BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
    WAIT(.02);
    END;
END;


//----Zoom Out----
Zoom_Out()
BEGIN
  IMAGE_WIDTH:=IMAGE_WIDTH-(IMAGE_WIDTH/100);
  IMAGE_HEIGHT:=IMAGE_HEIGHT-(IMAGE_HEIGHT/100);
  Clear_Background();
  BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
  WHILE ISKEYDOWN(45)
  DO
    IMAGE_WIDTH:=IMAGE_WIDTH-(IMAGE_WIDTH/100);
    IMAGE_HEIGHT:=IMAGE_HEIGHT-(IMAGE_HEIGHT/100);
    Clear_Background();
    BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
    WAIT(.02);
    END;
END;



//----Touch Gestures----
Touch_Gestures()
BEGIN
  X_AXIS_TOUCH:=(HMS→(MOUSE(2)));
  Y_AXIS_TOUCH:=(HMS→(MOUSE(3)));

  IF X_AXIS_TOUCH==-1
  THEN 
  ELSE
    REPEAT

      IF ((HMS→(MOUSE(4)))==3)
      THEN
        Touch_Zoom();
      END;

      IF ((HMS→(MOUSE(4)))==7)
      THEN
        Touch_Zoom();
      END;

      X_AXIS_TOUCH_SHIFT:=(HMS→(MOUSE(0)));
      Y_AXIS_TOUCH_SHIFT:=(HMS→(MOUSE(1)));

      IF X_AXIS_TOUCH_SHIFT==-1 
      THEN
        Read_Keyboard();
      END;

      IF Y_AXIS_TOUCH_SHIFT==-1
      THEN
        Read_Keyboard();
      END;

      IF X_AXIS_TOUCH_SHIFT>X_AXIS_TOUCH
      THEN
        X_AXIS_SHIFT:=X_AXIS_SHIFT+(X_AXIS_TOUCH_SHIFT-X_AXIS_TOUCH);
      END;

      IF Y_AXIS_TOUCH_SHIFT>Y_AXIS_TOUCH 
      THEN
        Y_AXIS_SHIFT:=Y_AXIS_SHIFT+(Y_AXIS_TOUCH_SHIFT-Y_AXIS_TOUCH);
      END;

      IF X_AXIS_TOUCH_SHIFT<X_AXIS_TOUCH
      THEN
        X_AXIS_SHIFT:=X_AXIS_SHIFT-(X_AXIS_TOUCH-X_AXIS_TOUCH_SHIFT);
      END;

      IF Y_AXIS_TOUCH_SHIFT<Y_AXIS_TOUCH
      THEN
        Y_AXIS_SHIFT:=Y_AXIS_SHIFT-(Y_AXIS_TOUCH-Y_AXIS_TOUCH_SHIFT);
      END;

      Clear_Background();
      BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
      X_AXIS_TOUCH:=X_AXIS_TOUCH_SHIFT;
      Y_AXIS_TOUCH:=Y_AXIS_TOUCH_SHIFT;

    UNTIL Y_AXIS_TOUCH_SHIFT==-1;
  END;
END;


//----Touch Zoom----
Touch_Zoom() 
BEGIN
  TZ_4:=0;
  REPEAT
    TZ_1:=(HMS→(MOUSE(0)));
    TZ_2:=(HMS→(MOUSE(5)));
    TZ_3:=(TZ_2-TZ_1);

    IF TZ_3>TZ_4
    THEN
      IMAGE_WIDTH:=IMAGE_WIDTH+((IMAGE_WIDTH/100)*TZ_4*TZ_ZOOM_SENSITIVITY);
      IMAGE_HEIGHT:=IMAGE_HEIGHT+((IMAGE_HEIGHT/100)*TZ_4*TZ_ZOOM_SENSITIVITY);
    END;

    IF TZ_4>TZ_3
    THEN
      IMAGE_WIDTH:=IMAGE_WIDTH-((IMAGE_WIDTH/100)*TZ_4*TZ_ZOOM_SENSITIVITY);
      IMAGE_HEIGHT:=IMAGE_HEIGHT-((IMAGE_HEIGHT/100)*TZ_4*TZ_ZOOM_SENSITIVITY);
    END;

    Clear_Background();
    BLIT_P(G1,X_AXIS_SHIFT,Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT);
    TZ_4:=TZ_3;

  UNTIL TZ_1==-1;
  Read_Keyboard();
END;


//----Clear Background----
Clear_Background()
BEGIN
  RECT_P(0,0,X_AXIS_SHIFT,240,#000000);
  RECT_P(IMAGE_WIDTH+X_AXIS_SHIFT,0,320,240,#000000);
  RECT_P(X_AXIS_SHIFT,0,IMAGE_WIDTH+X_AXIS_SHIFT,Y_AXIS_SHIFT,#000000);
  RECT_P(X_AXIS_SHIFT,IMAGE_HEIGHT+Y_AXIS_SHIFT,IMAGE_WIDTH+X_AXIS_SHIFT,360,#000000);
END;

//---------------------------------------------------
//---------------- Gallery Ends Here ----------------
//---------------------------------------------------