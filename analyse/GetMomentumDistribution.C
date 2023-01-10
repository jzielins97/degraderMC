/*************************************************
 * Macro for calculating pz vs pT distribution
 * and further analysing the degrader foil data
 ************************************************/

/*************************************************
Function for creating pz vs pT distribution for all
output files.
 Arguments:
 filename - name of the simulation output file
 maxPt        - maximum radial momentum that
                is trappable
 maxPz        - maximum momentum in the beam
                direction that is trappable
**********************************************/
void GetMomentumDistribution(TString filename){
  //open the output file
  TFile *oFile = new TFile("distributions.root","UPDATE");
  
  // create name for histogram from the filename
  TString stripName = filename;
  // remove the directory path from the filename
  Int_t index = stripName.Index("/");
  while(index != -1){
    stripName.Remove(0,index+1);
    index = stripName.Index("/");
  }
  // remove the file extension from the filename to get name for histograms
  stripName.ReplaceAll(".root","");
  
  // create 2D histogram for the pz vs pT
  TH2D *hpzvspt = new TH2D(TString::Format("%s_hpzvspt",stripName.Data()),"p_{z} vs p_{T};p_{T};p_{z}",1000,0,100,1000,0,100);
  
  //open the input file
  TFile *iFile; // pointer to the file
  iFile = new TFile(filename);
  if(iFile->IsZombie()){
    std::cout<<"ERROR: opening file ("<<filename<<")"<<std::endl;
    return -1;
  }

  // access NTuple(tree) from the file
  TTree *simTree; // tree with momentum values from the simulation
  Double_t pX, pY, pZ; // momentum values stored in the tree
  simTree = (TTree*)iFile->Get("fMomentum")->Clone();
  if(!simTree){
    std::cout<<"ERROR: couldn't find fMomentum NTuple in "<<filename<<std::endl;
    return -1;
  }
  simTree->SetBranchAddress("px_keV",&pX);
  simTree->SetBranchAddress("py_keV",&pY);
  simTree->SetBranchAddress("pz_keV",&pZ);
  
  // go one by one entry in the trree and fill the histogram
  for(int event=0;event<simTree->GetEntries();event++){ // loop over all antiprotons
    simTree->GetEntry(event);
    TVector3 pVec(pX,pY,pZ); // vector with momentum from the simulation
    hpzvspt->Fill(pVec.Pt(),pZ);
  }// end of the loop over all antiprotons
  
  // close the input file
  iFile->Close();

  // save histogram in output file
  oFile->cd();
  hpzvspt->Write();
  oFile->Close();

}

/*************************************************
 Function for calculating number of antiprotons
 with the Z momentum higher than pz as a function
 of pz (fPz).
 Arguments:
 hpzvspt      - 2D histogram of pz vs pt
 lowCutPt     - minimum pT for integral 
 highCutPt    - maximum pT for integral
**********************************************/
TH1D* CalculateIntegral(TH2D *hpzvspt, Double_t lowCutPt = -1, Double_t highCutPt = -1){
  // convert double values for cuts into bin numbers 
  Int_t lowCutPtBin = hpzvspt->ProjectionX()->FindBin(lowCutPt);
  Int_t highCutPtBin = highCutPt == -1 ? -1 : hpzvspt->ProjectionX()->FindBin(highCutPt);
  // create output histogram with same range and bin number as the 2D one
  Int_t nBins = hpzvspt->GetNbinsY();
  Double_t pzMin = hpzvspt->ProjectionY()->GetBinLowEdge(1); // bin 0 is underflow, bin 1 is the first bin with defined values
  Double_t pzMax = hpzvspt->ProjectionY()->GetBinLowEdge(nBins+1); // lower edge of the overflow bin is the higher edge of the last bin
  // create name for the output histogram
  TString histName(hpzvspt->GetName());
  if(histName.Contains("hpzvspt")) histName.ReplaceAll("hpzvspt","fPzVsPz");
  else histName+="_fPzVsPz";
  TH1D *hist = new TH1D(histName,"#bar{p} with p_{z} equal or higher;p_{z};#bar{p} count", nBins, pzMin,pzMax);
  // calculate integrals
  for(Int_t iBin = 1; iBin <= hist->GetNbinsX(); iBin++){
    Double_t integral = hpzvspt->Integral(lowCutPtBin,highCutPtBin,iBin,-1);
    hist->SetBinContent(iBin,integral);
  }
  return hist;
}

/*************************************************
 Function for calculating fPz for all 2D histograms
 storred in the provided file.
 Arguments:
 filename    - name of the file with 2D histograms
**********************************************/
void CalculatefPzFunctions(TString filename){
  //open the output file
  TFile *oFile = new TFile("fPzVsPz.root","RECREATE");
    
  //open the input file
  TFile *iFile; // pointer to the file
  iFile = new TFile(filename);
  if(iFile->IsZombie()){
    std::cout<<"ERROR: opening file ("<<filename<<")"<<std::endl;
    return -1;
  }
  auto listOfKeys = iFile->GetListOfKeys();

  TH1D *hist;
  for(int i=0;i<listOfKeys->GetEntries();i++){
    std::cout<<"Doing "<<listOfKeys->At(i)->GetName()<<"...";
    TH2D *hpzvspt = (TH2D*)iFile->Get(listOfKeys->At(i)->GetName())->Clone();
    hist = CalculateIntegral(hpzvspt);
    oFile->cd();
    hist->Write();
    delete hist;
    std::cout<<"done"<<std::endl;
  }
  iFile->Close();
  oFile->Close();
}

/************************************************/
/*************PLOTTING FUNCTIONS*****************/
/************************************************/

/************************************************/
void formatHistogram(TH1* hist, Int_t markerStyle, Int_t markerColor, Double_t markerSize=0.8){
  hist->SetMarkerColor(markerColor);
  hist->SetLineColor(markerColor);
  hist->SetMarkerStyle(markerStyle);
  hist->SetMarkerSize(markerSize);
}

void plot(Double_t mylarThickness){
  // turn off the stats display for the histograms
  gStyle->SetOptStat(0);
  
  // open file
  TFile *file = new TFile("fPzVsPz.root");

  // constants
  Double_t maxCount = 1e6;

  // create canvas to draw
  TCanvas* c1 = new TCanvas("c1",TString::Format("%.0fMYLAR",mylarThickness),10,10,850,800);
  c1->SetLeftMargin(0.15);
  c1->SetRightMargin(0.1);

  // legends
  TLegend *leg = new TLegend(0.75,0.9,0.9,0.65);
  leg->SetHeader("Parylene");
  
  // create first histogram
  TH1D* hist;
  hist = (TH1D*)file->Get(TString::Format("%.0fMYLAR_fPzVsPz",mylarThickness))->Clone();
  hist->SetTitle(TString::Format("%s (%.0f nm mylar)",hist->GetTitle(),mylarThickness));
  formatHistogram(hist, 20,kBlack); // full circle, black
  hist->GetXaxis()->SetTitle("p_{z} [keV]");
  hist->GetXaxis()->SetRangeUser(0,20);
  hist->GetXaxis()->SetMaxDigits(3);
  hist->GetYaxis()->SetRangeUser(0,maxCount);
  hist->GetYaxis()->SetMaxDigits(2);
  leg->AddEntry(hist,"0 nm","p");
  // leg->AddEntry(hist,TString::Format("%.0f nm",mylarThickness),"p");
  // draw it
  hist->Draw("P");

  for(int i=1; i<=5;i+=1){
    TH1D* hist2 = (TH1D*)file->Get(TString::Format("%dPARYLENE+%.0fMYLAR_fPzVsPz",i*100,mylarThickness))->Clone();
    // TH1D* hist2 = (TH1D*)file->Get(TString::Format("%.0fMYLAR_fPzVsPz",mylarThickness+i*20))->Clone();
    formatHistogram(hist2,24,i);
    leg->AddEntry(hist2,TString::Format("%d nm",i*100),"p");
    // leg->AddEntry(hist2,TString::Format("%.0f nm",mylarThickness+i*20),"p");
    hist2->Draw("SAME P");
  }

  leg->Draw();
  
  c1->SaveAs(TString::Format("%.0fMYLAR_fPzVsPz.png",mylarThickness));
  // c1->SaveAs("MYLAR_fPzVsPz.png");
}

void plot_capture(Double_t mylarThickness){
  // turn off the stats display for the histograms
  gStyle->SetOptStat(0);
  
  // open file
  TFile *file = new TFile("fPzVsPz.root");
  TFile *distributionFile = new TFile("distributions.root");

  // constants
  Double_t maxCount = 1e6;

  // create canvas to draw
  TCanvas* c1 = new TCanvas("c1",TString::Format("%.0fMYLAR",mylarThickness),10,10,850,800);
  c1->SetLeftMargin(0.15);
  c1->SetRightMargin(0.1);

  // legends
  TLegend *leg = new TLegend(0.75,0.9,0.9,0.65);
  leg->SetHeader("Parylene");
  
  // create first histogram
  Double_t pbarCount = 1;
  TH1D* hist = (TH1D*)file->Get(TString::Format("%.0fMYLAR_fPzVsPz",mylarThickness))->Clone();
  pbarCount = ((TH1D*)distributionFile->Get(TString::Format("%.0fMYLAR_hpzvspt",mylarThickness)))->GetEntries();
  hist->SetTitle(TString::Format("#bar{p} with pz equal or less (%.0f nm mylar)",mylarThickness));
  for(int iBin=1;iBin<=hist->GetNbinsX();iBin++) hist->SetBinContent(iBin,pbarCount-hist->GetBinContent(iBin));
  // hist->Scale(100./pbarCount);
  formatHistogram(hist, 20,kBlack); // full circle, black
  hist->GetXaxis()->SetTitle("p_{z} [keV]");
  hist->GetXaxis()->SetRangeUser(0,20);
  hist->GetXaxis()->SetMaxDigits(3);
  hist->GetYaxis()->SetTitle("#bar{p} count");
  hist->GetYaxis()->SetRangeUser(0,maxCount);
  hist->GetYaxis()->SetMaxDigits(3);
  leg->AddEntry(hist,"0 nm","p");
  // draw it
  hist->Draw("P");

  for(int i=1; i<=5;i+=1){
    TH1D* hist2 = (TH1D*)file->Get(TString::Format("%dPARYLENE+%.0fMYLAR_fPzVsPz",i*100,mylarThickness))->Clone();
    pbarCount = ((TH1D*)distributionFile->Get(TString::Format("%dPARYLENE+%.0fMYLAR_hpzvspt",i*100,mylarThickness)))->GetEntries();
    for(int iBin=1;iBin<=hist2->GetNbinsX();iBin++) hist2->SetBinContent(iBin,pbarCount- hist2->GetBinContent(iBin));
    // hist2->Scale(100./pbarCount);
    formatHistogram(hist2,24,i);
    leg->AddEntry(hist2,TString::Format("%i nm",i*100),"p");
    hist2->Draw("SAME P");
  }

  leg->Draw();

  c1->SaveAs(TString::Format("%.0fMYLAR_fPzVsPz-capture.png",mylarThickness));
}
