/*************************************************
 * Macro for analysing output files from the 
 * degrader foils simulations.
 ************************************************/

/**********************************************
 Function returns number of antiprotons with 
 momentum less than specified values 
 Arguments:
 filename - name of the simulation output file
 maxPt        - maximum radial momentum that
                is trappable
 maxPz        - maximum momentum in the beam
                direction that is trappable
**********************************************/
//-------------------------------------------->
Int_t SimpleCountAntiprotons(TString filename, Double_t maxPt, Double_t maxPz){
  //open the file
  TFile* file; // pointer to the file
  file = new TFile(filename);
  if(file->IsZombie()){
    std::cout<<"ERROR: opening file ("<<filename<<")"<<std::endl;
    return -1;
  }


  // access NTuple(tree) from the file
  TTree* simTree; // tree with momentum values from the simulation
  Double_t pX, pY, pZ; // momentum values stored in the tree
  simTree = (TTree*)file->Get("fMomentum")->Clone();
  if(!simTree){
    std::cout<<"ERROR: couldn't find fMomentum NTuple in "<<filename<<std::endl;
    return -1;
  }
  simTree->SetBranchAddress("px_keV",&pX);
  simTree->SetBranchAddress("py_keV",&pY);
  simTree->SetBranchAddress("pz_keV",&pZ);
  
  // count antiprotons
  Int_t counter = 0; // trappable antiprotons
  for(int event=0;event<simTree->GetEntries();event++){ // loop over all antiprotons
    simTree->GetEvent(event);
    TVector3 pVec(pX,pY,pZ); // vector with momentum from the simulation
    if(pVec.Pz() <= maxPz && pVec.Pt() <= maxPt) counter++;
  }// end of the loop over all antiprotons
  file->Close();
  return counter;
}
//--------------------------------------------<

/**********************************************
 Function for creating a graph of a number
 of trappable antiprotons, after passing
 through given material, as a function of
 thickness of the material
 Arguments:
 foilMaterial - name of the material (as saved
                in the sim output filename)
 from         - minimum thickness of the material
                (in nm)
 step         - step used for material thickness
 to           - maximum thickness of the material
                (in nm)
 maxPz        - maximum momentum in the beam
                direction that is trappable
 maxPt        - maximum radial momentum that
                is trappable
 bFieldFlag   - was simulation done with the bField?
**********************************************/
//-------------------------------------------->
void AnalyseSingleFoil(TString foilMaterial, Double_t from, Double_t step, Double_t to, Double_t maxPz=10, Double_t maxPt=10, bool bFieldFlag=true){
  TGraph* gr = new TGraph();
  // format the graph
  gr->SetName("trapPBarGr");
  gr->SetTitle(TString::Format("Number of #bar{p} with p_{Z}<%.1f, p_{T}<%.1f after passing through %s;nm of %s;Number of #bar{p}",maxPz, maxPt, foilMaterial.Data(),foilMaterial.Data()));
  gr->SetMarkerStyle(8); // full circle
  gr->SetMarkerColor(kRed); // black color
  
  Int_t maxPbars = 0;
  Double_t thickness = from;
  TString dirName;
  if(bFieldFlag) dirName = TString("withBField/");
  else dirName = TString("withoutBField/");

  do{
    Int_t nPbars = SimplaCountAntiprotons(dirName+TString::Format("%.0f%s.root",thickness,foilMaterial.Data()), maxPt, maxPz);
    gr->AddPoint(thickness,nPbars);
    if(nPbars > maxPbars) maxPbars = nPbars;
    std::cout<<thickness<<" "<<nPbars<<std::endl;
    thickness += step;
  }while(thickness <= to);


  //draw the graph and save it as png
  TCanvas* c = new TCanvas("c","graph",10,10,800,800);
  gr->GetYaxis()->SetMaxDigits(3);
  gr->GetYaxis()->SetRangeUser(0,1.1*maxPbars);
  gr->Draw("AP");

  // add axis with percentage
  Double_t percentageMin = 0 * 100./1e6;
  Double_t percentageMax = 1.1*maxPbars * 100./1e6;  
  std::cout<<percentageMin<<" "<<percentageMax<<std::endl;

  c->cd();
  // we need to create a second, transparent pad for the axis
  TPad *overlay = new TPad("overlay","",0,0,1,1);
  overlay->SetFillStyle(0);
  overlay->SetFillColor(0);
  overlay->SetFrameFillStyle(0);
  overlay->Draw("FA");
  overlay->cd();
  
  TGaxis *percentageAxis = new TGaxis(0.9,0.1,0.9,0.9,percentageMin,percentageMax,510,"+L");
  percentageAxis->SetTitle("Trappable #bar{p} in %");
  percentageAxis->SetLabelSize(0.02);
  percentageAxis->SetTitleSize(0.03);
  percentageAxis->Draw();
  c->SaveAs(TString::Format("graphs/%.0f-%.0f%s_pZ%.2fkeV_pT%.2fkeV.png",from,to,foilMaterial.Data(),maxPz,maxPt));
}
//--------------------------------------------<

/**********************************************
 Function for creating a graph of a number
 of trappable antiprotons, after passing
 through given material, as a function of
 thickness of the material
 Arguments:
 foilMaterial - name of the material (as saved
                in the sim output filename)
 from         - minimum thickness of the material
                (in nm)
 step         - step used for material thickness
 to           - maximum thickness of the material
                (in nm)
 maxPz        - maximum momentum in the beam
                direction that is trappable
 maxPt        - maximum radial momentum that
                is trappable
 bFieldFlag   - was simulation done with the bField?
**********************************************/
//-------------------------------------------->
void AnalyseResults(Double_t from, Double_t step, Double_t to, Double_t maxPz=10, Double_t maxPt=10, bool bFieldFlag=true){
  Double_t mylarThickness[6] = {1380,1400,1420,1480,1500,1520};
  Int_t markerStyle[6] = {20, 21, 34, 24, 25, 28};
  Int_t markerColor[6] = {1, 2, 3, 4, 6, 7};

  TMultiGraph* mg = new TMultiGraph();
  TLegend* leg = new TLegend(0.2,0.35,0.5,0.11);
  leg->SetHeader("MYLAR");
  Int_t maxPbars = 0;

  for(int i=0; i<6; i++){
    TGraph* gr = new TGraph();
    // format the graph
    gr->SetName(TString::Format("trapPBarGr_%.0f",mylarThickness[i]));
    gr->SetTitle(TString::Format("%.0f nm",mylarThickness[i]));
    gr->SetMarkerStyle(markerStyle[i]); // full circle
    gr->SetMarkerColor(markerColor[i]); // black color

    leg->AddEntry(gr,gr->GetTitle(),"p");
  
    Double_t thickness = from;
    TString dirName;
    if(bFieldFlag) dirName = TString("withBField/");
    else dirName = TString("withoutBField/");

    do{
      Int_t nPbars = SimplaCountAntiprotons(dirName+TString::Format("%.0fPARYLENE+%.0fMYLAR.root",thickness,mylarThickness[i]), maxPt, maxPz);
      if(nPbars > -1) gr->AddPoint(thickness,nPbars);
      if(nPbars > maxPbars) maxPbars = nPbars;
      std::cout<<thickness<<" "<<nPbars<<std::endl;
      thickness += step;
    }while(thickness <= to);
    mg->Add(gr);
  }

  //draw the graph and save it as png
  TCanvas* c = new TCanvas("c","graph",10,10,850,800);
  c->SetLeftMargin(0.15);
  c->SetRightMargin(0.1);
  mg->SetTitle(TString::Format("Number of #bar{p} with p_{Z}<%.1f keV, p_{T}<%.1f keV;nm of PARYLENE;Number of #bar{p}",maxPz, maxPt));
  mg->GetXaxis()->SetMaxDigits(3);
  mg->GetYaxis()->SetRangeUser(0,1.1*maxPbars);
  mg->GetYaxis()->SetMaxDigits(2);
  mg->Draw("AP");

  // add axis with percentage
  Double_t percentageMin = 0 * 100./1e6;
  Double_t percentageMax = 1.1*maxPbars * 100./1e6;  
  // std::cout<<percentageMin<<" "<<percentageMax<<std::endl;

  c->cd();
  // we need to create a second, transparent pad for the axis
  TPad *overlay = new TPad("overlay","",0,0,1,1);
  overlay->SetFillStyle(0);
  overlay->SetFillColor(0);
  overlay->SetFrameFillStyle(0);
  overlay->Draw("FA");
  overlay->cd();
  
  TGaxis *percentageAxis = new TGaxis(0.9,0.1,0.9,0.9,percentageMin,percentageMax,510,"+L");
  percentageAxis->SetTitle("Trappable #bar{p} in %");
  percentageAxis->SetLabelSize(mg->GetYaxis()->GetLabelSize());
  percentageAxis->SetLabelFont(mg->GetYaxis()->GetLabelFont());
  percentageAxis->SetTitleSize(mg->GetYaxis()->GetTitleSize());
  percentageAxis->SetTitleFont(mg->GetYaxis()->GetTitleFont());
  // percentageAxis->SetMaxDigits(2);
  percentageAxis->Draw();


  // draw legend
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetLineWidth(0);
  leg->Draw();

  c->SaveAs(TString::Format("graphs/parylene+mylar_pZ%.2fkeV_pT%.2fkeV.png",maxPz,maxPt));
}
//--------------------------------------------<
