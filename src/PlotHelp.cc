#include "../include/PlotHelp.h"
#include "../include/Plotter.h"
#include "../include/PlotTools.h"
#include <TH2.h>
using namespace std;

namespace PlotHelp {

TH1 * getEff(TFile *f, TString sN, TString num, TString den, float rebin = 0, int nR = 0, double * rebins = 0) {
	TH1 *hn = (TH1*)f->Get(sN+"_"+num);
	TH1 *hd = (TH1*)f->Get(sN+"_"+den);
	if (hn==0) {throw invalid_argument(sN+"_"+num+" does not exist"); return 0;}
	if (hd==0) {throw invalid_argument(sN+"_"+den+" does not exist"); return 0;}

    hn = (TH1*)hn->Clone();
    hd = (TH1*)hd->Clone();
    PlotTools::toOverflow(hn);
    PlotTools::toOverflow(hd);

    if(rebin > 0){
      PlotTools::rebin(hn,rebin);
      PlotTools::rebin(hd,rebin);
    } else if(rebins){
      hn = PlotTools::rebin(hn,nR,rebins);
      hd = PlotTools::rebin(hd,nR,rebins);
    }
    hn->Divide(hn,hd,1,1,"b");
    return hn;
}

TH1 * getTotBkgDist(vector<pair<TString,TFile*>> bkgs, TString hist) {

	if (bkgs.size() < 2) {cout<<"why u doin this? less than 2 samps"<<endl; return 0;}
	TH1 *h1 = (TH1F*)bkgs[0].second->Get(bkgs[0].first+"_"+hist);
	TH1 *hb = (TH1F*)h1->Clone();

	for (unsigned int i = 1; i < bkgs.size(); i++) {
		TH1 *h = (TH1*)bkgs[i].second->Get(bkgs[i].first+"_"+hist);
		hb->Add(h,1);
	}

	return hb;
}

TH1 * getEffHistByMX(TString num, TString den, TFile *f, vector<int> masses) {
	TH1 *hs = new TH1F(num+den,num+den,5000,0,5000);
	for (const auto& mass : masses) {
		TString m = TString::Format("m%i_",mass);
		double numF = ((TH1*)f->Get(m+num))->Integral();
		double denF = ((TH1*)f->Get(m+den))->Integral();
		hs->Fill(mass,numF/denF);
	}
	return hs;
}

void drawEffs(vector<pair<TString,TFile*>> samps, bool isSig, TString pre, vector<TString> sels, TString numS, TString denS,
		TString canName="slurm", float rebin=0, int nR = 0, double * rebins = 0) {

	vector<int> masses = {800,900,1000,1200,1400,1600,1800,2000,2500,3000,3500,4000,4500};
	Plotter *p = new Plotter();

	if (isSig) {

		if (samps.size() != 1) {
	        throw invalid_argument("Only provide 1 file (rad or blk) for signal efficiencies");
		}
		for (const auto& sel : sels) {
			TH1 *h = getEffHistByMX(pre+sel+numS,denS,samps[0].second,masses);

			p->addHist(h,sel,-1,1,4,20,1,true,false);
		}
		p->setXTitle("M_{X}");
		p->draw(false,canName);

	} else {

		TH1 *hd = getTotBkgDist(samps,denS);
		if (hd==0) {cout<<"bad den: "<<denS<<endl; return;}

		hd = (TH1*)hd->Clone();
		PlotTools::toOverflow(hd);
		PlotTools::toUnderflow(hd);
	    if(rebin > 0) PlotTools::rebin(hd,rebin);
	    else if(rebins) hd = PlotTools::rebin(hd,nR,rebins);

		for (const auto& sel : sels) {
			TH1 *hn = getTotBkgDist(samps,pre+sel+numS);
	    	if (hn==0) {cout<<"bad num: "<<pre+sel+numS<<endl; continue;}

	    	hn = (TH1*)hn->Clone();
	    	PlotTools::toOverflow(hn);
	    	PlotTools::toUnderflow(hn);

	        if(rebin > 0) PlotTools::rebin(hn,rebin);
	        else if(rebins) hn = PlotTools::rebin(hn,nR,rebins);

	        hn->Divide(hn,hd,1,1,"b");
	        p->addHist(hn,sel);
		}
	    p->draw(false,canName);
	}
}

TH2 * getSoBHist(pair<TString,TFile*> sSamp, vector<pair<TString,TFile*>> bSamps, int type,
		vector<TString> wps) {
// type 0 = IP, 1 = ID, 2 = iso

	int dim = wps.size();
	TH2 *h = new TH2("h","h",dim,0,dim,dim,0,dim);

	TString pre, sel, post;

	for(unsigned int wp1=0; wp1<wps.size(); wp1++) for(unsigned int wp2=0; wp2<wps.size(); wp2++) {
		if (type == 1) {
			pre = "passIP_";
			sel = "ID_e"+wp1;
			post = "_miniIso0p2_ht";
		} else if (type == 2) {
			pre = "passIP_ID_eMVA_mM_";
		}

		TString shist = sSamp.first+"_";
		TString bhist = "";
		TH1 *hhs = (TH1*)sSamp.second->Get(shist);
		TH1 *hhb = getTotBkgDist(bSamps,bhist);

		double sob = hhs->Integral() / hhb->Integral();
		h->SetBinContent(wp1+1,wp2+1,sob);
	}

	return h;
}


}
