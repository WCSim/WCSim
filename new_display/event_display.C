void event_display(TString filename="./outfiles/input_1_1_0.5_5.0_0.root"){
    
  
  // filename = "temp.root";
  Xin_Draw *test = new Xin_Draw(filename,gClient->GetRoot(), 800, 800);
    
 //  //batch mode
//   for (Int_t i=0;i!=10;i++){
//     test->SetEventNo(i);
//     test->DoDraw();
//     test->DoSave();
//   }
//   test->DoExit();
}
