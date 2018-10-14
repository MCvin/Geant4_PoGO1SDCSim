//
//  DetectorConstruction.cc 2015-11-03  Maxime Chauvin
//

#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4ThreeVector.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trap.hh"
#include "G4Polyhedra.hh"
#include "G4Polycone.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4PVPlacement.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

DetectorConstruction::DetectorConstruction()
{
  // define the geometry parameters
  //FastLength = 98.0*mm;
  FastLength = 120.0*mm;
  FastWidth = 13.875*mm;

  // define materials
  DefineMaterials();

  // define some rotation matrices
  DefineRotations();

  // geometry overlap check
  overlap_check = true;
}

DetectorConstruction::~DetectorConstruction(){}

void DetectorConstruction::DefineMaterials()
{
  G4NistManager * nist_manager = G4NistManager::Instance();
  // elements
  G4Element *elementSi = nist_manager->FindOrBuildElement("Si");
  G4Element *elementFe = nist_manager->FindOrBuildElement("Fe");
  G4Element *elementCu = nist_manager->FindOrBuildElement("Cu");
  G4Element *elementMn = nist_manager->FindOrBuildElement("Mn");
  G4Element *elementMg = nist_manager->FindOrBuildElement("Mg");
  G4Element *elementCr = nist_manager->FindOrBuildElement("Cr");
  G4Element *elementZn = nist_manager->FindOrBuildElement("Zn");
  G4Element *elementTi = nist_manager->FindOrBuildElement("Ti");
  G4Element *elementAl = nist_manager->FindOrBuildElement("Al");
  // materials
  nist_manager->FindOrBuildMaterial("G4_AIR");
  nist_manager->FindOrBuildMaterial("G4_Pb");
  nist_manager->FindOrBuildMaterial("G4_BGO");
  nist_manager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  //nist_manager->BuildMaterialWithNewDensity("Fast","G4_PLASTIC_SC_VINYLTOLUENE", 1.0*g/cm3);
  nist_manager->FindOrBuildMaterial("G4_PLEXIGLASS");
  // Aluminium alloy 6061
  G4Material* Al_6061 = new G4Material("Al_6061", 2.70*g/cm3, 9);
  Al_6061->AddElement(elementSi,  0.6*perCent);  //0.6
  Al_6061->AddElement(elementFe,  0.5*perCent);  //1.1
  Al_6061->AddElement(elementCu,  0.3*perCent);  //1.4
  Al_6061->AddElement(elementMn,  0.1*perCent);  //1.5
  Al_6061->AddElement(elementMg,  1.0*perCent);  //2.5
  Al_6061->AddElement(elementCr,  0.2*perCent);  //2.7
  Al_6061->AddElement(elementZn,  0.2*perCent);  //2.9
  Al_6061->AddElement(elementTi,  0.1*perCent);  //3.0
  Al_6061->AddElement(elementAl, 97.0*perCent);  //100
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Materials
  G4Material * air_1atm = G4Material::GetMaterial("G4_AIR");
  G4Material * Al_6061 = G4Material::GetMaterial("Al_6061");
  G4Material * Pb = G4Material::GetMaterial("G4_Pb");
  G4Material * BGO = G4Material::GetMaterial("G4_BGO");
  G4Material * EJ204 = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  //G4Material * EJ204 = G4Material::GetMaterial("Fast");
  G4Material * Plexi = G4Material::GetMaterial("G4_PLEXIGLASS");

  //-------------------- Create the World Volume ----------------------------------
  const G4double WorldSize  = 600.0*cm; 
  G4Box* s_World = new G4Box("World", WorldSize/2, WorldSize/2, WorldSize/2);
  G4LogicalVolume* l_World = new G4LogicalVolume(s_World, air_1atm, "World");
  G4VPhysicalVolume* p_World = new G4PVPlacement(0,	G4ThreeVector(), l_World, "World", NULL, false, 0, overlap_check);

  //-------------------- Create POGO mother volume --------------------------------
  G4Box* s_Pogo = new G4Box("Pogo", 95*cm,95*cm,95*cm);
  G4LogicalVolume* l_Pogo = new G4LogicalVolume(s_Pogo, air_1atm, "Pogo");
  p_Pogo = new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm), l_Pogo, "Pogo", l_World, false, 0, overlap_check);

  //-------------------- Now construct the lead cave ----------------------------
  G4Box* s_PbCave_out = new G4Box("PbCave_out", 100*mm,75*mm,325*mm);
  G4Box* s_PbCave_in = new G4Box("PbCave_in", 50*mm,25*mm,275*mm);
  G4SubtractionSolid* s_PbCave_nohole = new G4SubtractionSolid("PbCave_nohole", s_PbCave_out, s_PbCave_in);
  G4Tubs* s_PbCave_hole = new G4Tubs("PbCave_hole", 0.0*cm, 3.0/2.0*mm, 26.0*mm, 0.*deg, 360.*deg);
  G4SubtractionSolid* s_PbCave = new G4SubtractionSolid("PbCave", s_PbCave_nohole, s_PbCave_hole, 0, G4ThreeVector(0.0*mm,-8.0*mm,300.0*mm));
  G4LogicalVolume* l_PbCave = new G4LogicalVolume(s_PbCave, Pb, "PbCave");
  new G4PVPlacement(0, G4ThreeVector(0.0*mm,10.0*mm,-140.0*mm), l_PbCave, "PbCave", l_Pogo, false, 0, overlap_check);

  //-------------------- Now construct the source collimator --------------------
  G4Box* s_PbColl_bloc = new G4Box("PbColl_bloc", 25*mm,25*mm,13.0/2.0*mm);
  G4Tubs* s_PbColl_hole = new G4Tubs("PbColl_hole", 0.0*cm, 2.0/2.0*mm, 14.0/2.0*mm, 0.*deg, 360.*deg);
  G4SubtractionSolid* s_PbColl = new G4SubtractionSolid("PbColl", s_PbColl_bloc, s_PbColl_hole);
  G4LogicalVolume* l_PbColl = new G4LogicalVolume(s_PbColl, Pb, "PbColl");
  new G4PVPlacement(0, G4ThreeVector(0.0*mm,2.0*mm,191.5*mm), l_PbColl, "PbColl", l_Pogo, false, 0, overlap_check);


  //-------------------- Now construct 1 detector -------------------------------
  // SDC mother volume
  const G4String SDCName = "SDCUnit";
  const G4double SDCZ[2] = {-22.0*cm,FastLength};
  const G4double SDCInner[2] = {0.0*cm,0.0*cm};
  const G4double SDCOuter[2] = {1.499*cm,1.499*cm};
  G4Polyhedra* s_SDC = new G4Polyhedra(SDCName, 0.*deg, 360.*deg, 6, 2, SDCZ, SDCInner, SDCOuter);
  G4LogicalVolume* l_SDC = new G4LogicalVolume(s_SDC, air_1atm, SDCName);

      // fast scintillator
      const G4String FastName = "FastScintillator";
      const G4double FastZ[2] = {0.0*cm,FastLength};
      const G4double FastInner[2] = {0.0*cm,0.0*cm};
      const G4double FastOuter[2] = {FastWidth,FastWidth};
      G4Polyhedra* s_Fast = new G4Polyhedra(FastName, 0.*deg, 360.*deg, 6, 2, FastZ, FastInner, FastOuter);
      G4LogicalVolume* l_Fast = new G4LogicalVolume(s_Fast, EJ204, FastName);
      //G4LogicalVolume* l_Fast = new G4LogicalVolume(s_Fast, Plexi, FastName);
      new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm), l_Fast, FastName, l_SDC, false, 0, overlap_check);

      // bottom BGO
      const G4String BGObottomName = "BGObottom";
      const G4double BGOZ[2] = {0.0*cm,4.0*cm};
      const G4double BGOInner[2] = {0.0*cm,0.0*cm};
      const G4double BGOOuter[2] = {1.425*cm,1.425*cm};
      G4Polyhedra* s1_BGO = new G4Polyhedra("s1_BGO", 0.*deg, 360.*deg, 6, 2, BGOZ, BGOInner, BGOOuter);
      const G4double BGOCylinderZ[4] = {0.0*cm,1.2*cm,2.0*cm,4.0*cm};
      const G4double BGOCylinderInner[4] = {0.0*cm,0.0*cm,0.0*cm,0.0*cm};
      const G4double BGOCylinderOuter[4] = {1.15*cm,1.15*cm,1.612*cm,1.612*cm};
      G4Polycone* s2_BGO = new G4Polycone("s2_BGO", 0.*deg, 360.*deg, 4, BGOCylinderZ, BGOCylinderInner, BGOCylinderOuter);
      G4IntersectionSolid* s_BGObottom = new G4IntersectionSolid(BGObottomName, s1_BGO, s2_BGO);
      G4LogicalVolume* l_BGObottom = new G4LogicalVolume(s_BGObottom, BGO, BGObottomName);
      //G4LogicalVolume* l_BGObottom = new G4LogicalVolume(s_BGObottom, Plexi, BGObottomName);
      new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-4.0*cm), l_BGObottom, BGObottomName, l_SDC, false, 0, overlap_check);

      // PM Tube
      const G4String PMName = "PMTube";
      G4Tubs* s_PM = new G4Tubs(PMName, 0.*cm, 1.15*cm, 9.0*cm, 0.*deg, 360.*deg);
      G4LogicalVolume* l_PM = new G4LogicalVolume(s_PM, Al_6061, PMName);
      new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-13.0*cm), l_PM, PMName, l_SDC, false, 0, overlap_check);


  // place 1 SDC unit
  new G4PVPlacement(0, G4ThreeVector(0.0*mm,0.0*mm,0.0*mm), l_SDC, SDCName, l_Pogo, false, 0, overlap_check);


  //-------------------- Now define the sensitive detectors ----------------------------
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

    fastSD = (TrackerSD*)SDman->FindSensitiveDetector("fastSD", false);
    if (fastSD) delete fastSD;
      fastSD = new TrackerSD("fastSD");
      SDman->AddNewDetector(fastSD);
      l_Fast->SetSensitiveDetector(fastSD);

    BGObottomSD = (TrackerSD*)SDman->FindSensitiveDetector("bottomBGOSD", false);
    if (BGObottomSD) delete BGObottomSD;
      BGObottomSD = new TrackerSD("bottomBGOSD");
      SDman->AddNewDetector(BGObottomSD);
      l_BGObottom->SetSensitiveDetector(BGObottomSD);

    SASSD = new TrackerSD("SASSD");
      SDman->AddNewDetector(SASSD);

  //-------------------- Now define the visualization ----------------------------
  G4VisAttributes* colourWhite = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  G4VisAttributes* colourLightBlue = new G4VisAttributes(G4Colour(0.6,0.6,1.0));
  G4VisAttributes* colourGrey = new G4VisAttributes(G4Colour(0.8,0.8,0.8));
  G4VisAttributes* colourRed = new G4VisAttributes(G4Colour(1.0,0.4,0.4));
  colourWhite->SetForceSolid(true);
  colourLightBlue->SetForceSolid(true);
  colourGrey->SetForceSolid(true);
  colourRed->SetForceSolid(true);

  l_Fast->SetVisAttributes(colourLightBlue);
  l_BGObottom->SetVisAttributes(colourRed);
  l_PM->SetVisAttributes(colourWhite);

  l_World->SetVisAttributes(G4VisAttributes::Invisible);
  l_Pogo->SetVisAttributes(G4VisAttributes::Invisible);
  l_SDC->SetVisAttributes(G4VisAttributes::Invisible);

  return p_World;
}
