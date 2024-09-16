#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
    G4NistManager *nist = G4NistManager::Instance();

    //
    //GAGG
    //

    const G4int nEntries=2;
        G4double PhotonEnergy[nEntries]={1.0*eV,7.0*eV};
        G4double GAGGRefractionIndex[nEntries]={1.9,1.9};
        G4double GAGGAbsorptionLength[nEntries]={10.*cm,10.*cm};
        G4double ScintFast[nEntries]={1.0,1.0};
        G4double rindexWorld[2] = {1.0,1.0};

    G4Material* GAGG = new G4Material("GAGG", 6.63*g/cm3, 4);
        GAGG->AddElement(nist->FindOrBuildElement("Gd"), 3);
        GAGG->AddElement(nist->FindOrBuildElement("Al"), 2);
        GAGG->AddElement(nist->FindOrBuildElement("Ga"), 3);
        GAGG->AddElement(nist->FindOrBuildElement("O" ), 12);

  G4MaterialPropertiesTable* GAGGMPT = new G4MaterialPropertiesTable();
    GAGGMPT->AddProperty("RINDEX",PhotonEnergy,GAGGRefractionIndex,nEntries,true);
    GAGGMPT->AddProperty("ABSLENGTH",PhotonEnergy,GAGGAbsorptionLength,nEntries,true);
    GAGGMPT->AddProperty("FASTCOMPONENT",PhotonEnergy,ScintFast,nEntries,true);

    GAGGMPT->AddConstProperty("SCINTILLATIONYIELD",54000./MeV,true);
    GAGGMPT->AddConstProperty("RESOLUTIONSCALE",0.05,true);
    GAGGMPT->AddConstProperty("FASTTIMECONSTANT",94.*ns,true);
    GAGGMPT->AddConstProperty("YIELDRATIO",1.,true);

  GAGG->SetMaterialPropertiesTable(GAGGMPT);

    G4Material *worldMat = nist -> FindOrBuildMaterial("G4_AIR");

    G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
    mptWorld->AddProperty("RINDEX",PhotonEnergy,rindexWorld,2);

    worldMat->SetMaterialPropertiesTable(mptWorld);

  //
  // World
  //

    G4Box *solidWorld = new G4Box("solidWorld",0.5*m,0.5*m,0.5*m);

    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
    
    G4VPhysicalVolume *physWorld = new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicWorld,"physWorld",0,false,0,true);

  //
  // Shape1
  //

  // 定义圆柱体的尺寸参数
  G4double innerRadius_01 = 0;
  G4double outerRadius_01 = 0.1* m; // 直径为2mm，半径为1mm
  G4double height_01 = 0.2 * m;

  // 创建GAGG闪烁体圆柱
  auto solidShape1 = new G4Tubs("Shape1_GAGG", innerRadius_01, outerRadius_01, height_01/2.0, 0.0, 360.0 * deg);

  auto logicShape1_GAGG = new G4LogicalVolume(solidShape1, GAGG, "Shape1_GAGG");

  G4ThreeVector pos1 = G4ThreeVector(0, 0, -0.35*m);

  auto physShape1_GAGG = new G4PVPlacement(nullptr, pos1, logicShape1_GAGG, "Shape1_GAGG", logicWorld, false, 0, true);

// 定义铝材料
G4Material* aluminum = nist->FindOrBuildMaterial("G4_Al");

// 定义铝层几何体
G4double outerRadius_Al = outerRadius_01 + 0.01 * m; // 外半径增加1cm
G4Tubs* solidAlLayer = new G4Tubs("AlLayer", outerRadius_01, outerRadius_Al, height_01/2.0, 0.0, 360.0*deg);

// 创建铝层逻辑体积
G4LogicalVolume* logicAlLayer = new G4LogicalVolume(solidAlLayer, aluminum, "logicAlLayer");

// 创建铝层材料属性表
G4MaterialPropertiesTable* aluminumMPT = new G4MaterialPropertiesTable();

// 设置铝层的反射率为1，即完全反射
G4double reflectivity[] = {1.0};
G4double photonEnergy[] = {1.0*eV};
aluminumMPT->AddProperty("REFLECTIVITY", photonEnergy, reflectivity, 1);

// 设置铝层的边界条件为OPTICAL
aluminum->SetMaterialPropertiesTable(aluminumMPT);

// 将铝层放置在GAGG闪烁体外面
new G4PVPlacement(nullptr, pos1, logicAlLayer, "physAlLayer", logicWorld, false, 0, true);



  //
  // Detector
  //
  
    G4Box *solidDetector = new G4Box("solidDetector",0.005*m,0.005*m,0.01*m);

    logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");

    for(G4int i = 0; i < 100; i++)
    {
        for(G4int j = 0; j < 100; j++)
        {
            G4VPhysicalVolume *physDetector = new G4PVPlacement(0,G4ThreeVector(-0.5*m+(i+0.5)*m/100,-0.5*m+(j+0.5)*m/100,0.49*m),
                                                                logicDetector,"physDetector",logicWorld,false,j+i*100,true);

        }
    }

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
    
    logicDetector->SetSensitiveDetector(sensDet);

}