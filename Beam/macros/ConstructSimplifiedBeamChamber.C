// $Id: $

/*!
 * \file ConstructBeamEnvelope.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include <TEveGeoNode.h>
#include <TEveManager.h>
#include <TGLClip.h>
#include <TGLUtil.h>
#include <TGLViewer.h>
#include <TGeoManager.h>
#include <TGeoXtru.h>
#include <TROOT.h>

using namespace std;

//! This function construct a enclosure volume that can contain the beam pipe structures
//! It define the vacuum region and preferably to be as close to the outer surface of the engineering model as possible
//! In this simple implementation, this is just a simple TGeoPcon that is 5mm larger than the highest radius point of the pipe
void ConstructSimplifiedBeamChamber()
{
  // configuration constants
  const int n_seg = 128;
  bool do_clip = false;

  // EIC beam chamber measurements, Version tag [Detector Chamber 210813 Far FPA]
  const double Be_Section_Pos = 67;
  const double Be_Section_Neg = -80;
  const double Be_radius = 3.1;
  const double hardron_chamber_far_z = 500;
  const double hardron_chamber_far_z_max_vac_R = 24.7;
  const double hardron_chamber_far_z_x_center = 0.5 * (Be_radius - hardron_chamber_far_z_max_vac_R);
  const double hardron_chamber_far_z_vac_radius = 0.5 * (Be_radius + hardron_chamber_far_z_max_vac_R);

  // EIC beam chamber measurements, Version tag [Detector Chamber 210222 ]
  const double hardron_chamber_far_z_thickness = 4.116e-1;
  const double hardron_chamber_near_z_thickness = 6.454 / 2 - Be_radius;

  const double electron_chamber_far_z = -463;
  const double electron_chamber_far_z_max_vac_R = 2.057 + tan(25e-3) * (-electron_chamber_far_z);
  const double electron_chamber_far_z_x_center = 0.5 * (-Be_radius + electron_chamber_far_z_max_vac_R);
  const double electron_chamber_far_z_vac_radius = 0.5 * (Be_radius + electron_chamber_far_z_max_vac_R);

  // An approximation of unit circle
  vector<Double_t> x_circle(n_seg);
  vector<Double_t> y_circle(n_seg);
  for (int i = 0; i < n_seg; ++i)
  {
    const double phi = -(TMath::TwoPi() / n_seg * i);
    x_circle[i] = cos(phi);
    y_circle[i] = sin(phi);
  }

  // Create the geometry
  TEveManager::Create();

  // gStyle->SetCanvasPreferGL(true);
  TGeoManager *geom = new TGeoManager("BeamEnvelope", "BeamEnvelope");
  //--- define some materials
  TGeoMaterial *matVacuum = new TGeoMaterial("VACUUM", 0, 0, 0);
  // tricky name as we replace the Al with Fe for radiation shielding. It also help display coloring in G4
  TGeoMaterial *matIRON = new TGeoMaterial("Fe_Replacing_ALUMINUM_Chamber", 55.85, 26, 7.87);
  //   //--- define some media
  TGeoMedium *Vacuum = new TGeoMedium("VACUUM", 1, matVacuum);
  TGeoMedium *IRON = new TGeoMedium("Fe_Replacing_ALUMINUM_Chamber", 1, matIRON);

  //--- make the top container volume
  Double_t worldx = 10000.;
  Double_t worldy = 10000.;
  Double_t worldz = 10000.;
  TGeoVolume *top = geom->MakeBox("TOP", Vacuum, worldx, worldy, worldz);
  geom->SetTopVolume(top);

  // hadron chambers
  TGeoVolume *HadronForwardEnvelope = gGeoManager->MakeXtru("HadronForwardChamber", IRON, 2);
  TGeoXtru *xtru = (TGeoXtru *) (HadronForwardEnvelope->GetShape());
  assert(xtru);
  xtru->DefinePolygon(n_seg, x_circle.data(), y_circle.data());
  // Be beam pipe interface
  xtru->DefineSection(0, Be_Section_Pos,
                      0, 0,
                      Be_radius + hardron_chamber_near_z_thickness);
  // Be far-z interface
  xtru->DefineSection(1, hardron_chamber_far_z,
                      hardron_chamber_far_z_x_center, 0,
                      hardron_chamber_far_z_vac_radius + hardron_chamber_far_z_thickness);
  top->AddNode(HadronForwardEnvelope, 1);

  TGeoVolume *HadronForwardVac = gGeoManager->MakeXtru("HadronForwardVac", Vacuum, 2);
  xtru = (TGeoXtru *) (HadronForwardVac->GetShape());
  assert(xtru);
  xtru->DefinePolygon(n_seg, x_circle.data(), y_circle.data());
  // Be beam pipe interface
  xtru->DefineSection(0, Be_Section_Neg,
                      0, 0,
                      Be_radius);
  // Be far-z interface
  xtru->DefineSection(1, hardron_chamber_far_z,
                      hardron_chamber_far_z_x_center, 0,
                      hardron_chamber_far_z_vac_radius);
  HadronForwardEnvelope->AddNode(HadronForwardVac, 1);

  // electron chambers
  TGeoVolume *ElectronForwardChamber = gGeoManager->MakeXtru("ElectronForwardChamber", IRON, 2);
  xtru = (TGeoXtru *) (ElectronForwardChamber->GetShape());
  assert(xtru);
  xtru->DefinePolygon(n_seg, x_circle.data(), y_circle.data());
  // Be beam pipe interface
  xtru->DefineSection(1, Be_Section_Neg,
                      0, 0,
                      Be_radius + hardron_chamber_near_z_thickness);
  // Be far-z interface
  xtru->DefineSection(0, electron_chamber_far_z,
                      electron_chamber_far_z_x_center, 0,
                      electron_chamber_far_z_vac_radius + hardron_chamber_far_z_thickness);
  top->AddNode(ElectronForwardChamber, 2);

  TGeoVolume *ElectronForwardVac = gGeoManager->MakeXtru("ElectronForwardVac", Vacuum, 2);
  xtru = (TGeoXtru *) (ElectronForwardVac->GetShape());
  assert(xtru);
  xtru->DefinePolygon(n_seg, x_circle.data(), y_circle.data());
  // Be beam pipe interface
  xtru->DefineSection(1, Be_Section_Neg,
                      0, 0,
                      Be_radius);
  // Be far-z interface
  xtru->DefineSection(0, electron_chamber_far_z,
                      electron_chamber_far_z_x_center, 0,
                      electron_chamber_far_z_vac_radius);
  ElectronForwardChamber->AddNode(ElectronForwardVac, 1);

  //--- close the geometry
  geom->CloseGeometry();
  //--- draw the ROOT box.

  assert(gGeoManager);

  gGeoManager->Export("ConstructSimplifiedBeamChamber.gdml");

  if (!gROOT->GetListOfGeometries()->FindObject(gGeoManager))
    gROOT->GetListOfGeometries()->Add(gGeoManager);
  if (!gROOT->GetListOfBrowsables()->FindObject(gGeoManager))
    gROOT->GetListOfBrowsables()->Add(gGeoManager);
  //  gGeoManager->UpdateElements();

  TGeoNode *current = gGeoManager->GetCurrentNode();
  //Alternate drawing
  //current->GetVolume()->Draw("ogl");
  //Print the list of daughters
  current->PrintCandidates();
  for (int igeom = 0; igeom < current->GetNdaughters(); igeom++)
  {
    TGeoNode *geo_node = (TGeoNode *) current->GetNodes()->UncheckedAt(igeom);
    geo_node->GetVolume()->VisibleDaughters(kFALSE);
    geo_node->GetVolume()->SetTransparency(2);
  }
  TEveGeoTopNode *eve_node = new TEveGeoTopNode(gGeoManager, current);
  eve_node->SetVisLevel(6);
  gEve->AddGlobalElement(eve_node);
  gEve->FullRedraw3D(kTRUE);

  // EClipType not exported to CINT (see TGLUtil.h):
  // 0 - no clip, 1 - clip plane, 2 - clip box
  TGLViewer *v = gEve->GetDefaultGLViewer();
  if (do_clip)
  {
    v->GetClipSet()->SetClipType(TGLClip::kClipPlane);
  }
  v->ColorSet().Background().SetColor(kMagenta + 4);
  v->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
  v->RefreshPadEditor(v);

  v->CurrentCamera().RotateRad(-1.6, 0.);
  v->DoDraw();
}
