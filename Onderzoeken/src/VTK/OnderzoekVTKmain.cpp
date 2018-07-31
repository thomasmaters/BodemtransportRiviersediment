/*
 * OnderzoekVTKmain.cpp
 *
 *  Created on: 31 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */
#include <vtk-8.1/vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingContextOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
#include <vtk-8.1/vtkVersion.h>
#include <vtk-8.1/vtkRenderer.h>
#include <vtk-8.1/vtkRenderWindow.h>
#include <vtk-8.1/vtkSmartPointer.h>
#include <vtk-8.1/vtkChartXY.h>
#include <vtk-8.1/vtkPlot.h>
#include <vtk-8.1/vtkTable.h>
#include <vtk-8.1/vtkIntArray.h>
#include <vtk-8.1/vtkContextView.h>
#include <vtk-8.1/vtkContextScene.h>
#include <vtk-8.1/vtkRenderWindowInteractor.h>
#include <vtk-8.1/vtkTextActor.h>
#include <vtk-8.1/vtkTextProperty.h>
#include <vtk-8.1/vtkButtonWidget.h>
#include <vtk-8.1/vtkTexturedButtonRepresentation2D.h>
#include <vtk-8.1/vtkImageActor.h>
#include <vtk-8.1/vtkImageData.h>

#include <random>
#include <chrono>

#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()


void CreateImage(vtkSmartPointer<vtkImageData> image,
                 unsigned char* color1,
                 unsigned char* color2)
{
  // Specify the size of the image data
  image->SetDimensions(10, 10, 1);
#if VTK_MAJOR_VERSION <= 5
  image->SetNumberOfScalarComponents(3);
  image->SetScalarTypeToUnsignedChar();
#else
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
#endif
  int* dims = image->GetDimensions();

  // Fill the image with
  for (int y = 0; y < dims[1]; y++)
  {
    for (int x = 0; x < dims[0]; x++)
    {
      unsigned char* pixel =
        static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
      if (x < 5)
      {
        pixel[0] = color1[0];
        pixel[1] = color1[1];
        pixel[2] = color1[2];
      }
      else
      {
        pixel[0] = color2[0];
        pixel[1] = color2[1];
        pixel[2] = color2[2];
      }
    }
  }
}

class MyClass
{
  public:
    void KeypressCallbackFunction(vtkObject*,
                                  long unsigned int vtkNotUsed(eventId),
                                  void* vtkNotUsed(callData))
  {
    std::cout << "Caught event in MyClass" << std::endl;
  }

};

// Monthly circulation data
int main(int, char*[] )
{
  // Set up a 2D scene, add an XY chart to it
  VTK_CREATE(vtkContextView, view);
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  view->GetRenderWindow()->SetSize(400, 300);
  VTK_CREATE(vtkChartXY, chart);
//  view->GetScene()->AddItem(chart);

  // Create a table with some points in it...
  VTK_CREATE(vtkTable, table);

  VTK_CREATE(vtkIntArray, tblIndex);
  tblIndex->SetName("Index");
  table->AddColumn(tblIndex);

  VTK_CREATE(vtkIntArray, arrMonth);
  arrMonth->SetName("Month");
  table->AddColumn(arrMonth);

  auto start = std::chrono::steady_clock::now();
  table->SetNumberOfRows(500);
  for (std::size_t i = 0; i < 500; ++i)
  {
    table->SetValue(i,0,i);
    table->SetValue(i,1, std::rand() % 5);
  }

  auto end = std::chrono::steady_clock::now();
  auto diff = end - start;
  std::cout << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
  // Add multiple line plots, setting the colors etc
  vtkPlot *line = 0;

  line = chart->AddPlot(vtkChart::BAR);
  line->SetInputData(table, 0, 1);
  line->SetColor(0, 255, 0, 255);

  vtkSmartPointer<vtkTextProperty> textProperty =
    vtkSmartPointer<vtkTextProperty>::New();
  textProperty->SetFontSize(16);
  textProperty->SetColor(1.0, 0.0, 0.0);
  textProperty->SetJustificationToCentered();

  vtkSmartPointer<vtkTextActor> textActor =
    vtkSmartPointer<vtkTextActor>::New();
  textActor->SetInput ( "Hello world" );
  textActor->SetPosition2 ( 10, 40 );
  textActor->SetTextProperty(textProperty);

  unsigned char banana[3] = { 227, 207, 87 };
  unsigned char tomato[3] = { 255, 99, 71 };
  VTK_CREATE(vtkButtonWidget,buttonWidget);
  VTK_CREATE(vtkTexturedButtonRepresentation2D, representation);
  VTK_CREATE(vtkImageData,image);
  VTK_CREATE(vtkImageData,image2);
  CreateImage(image, tomato, banana);
  CreateImage(image2, banana, tomato);
  representation->SetNumberOfStates(2);
  representation->SetButtonTexture(0, image);
  representation->SetButtonTexture(1, image2);
  buttonWidget->SetInteractor(view->GetInteractor());
  buttonWidget->SetRepresentation(representation);
  vtkSmartPointer<vtkCoordinate> upperRight =
    vtkSmartPointer<vtkCoordinate>::New();
  upperRight->SetCoordinateSystemToNormalizedDisplay();
  upperRight->SetValue(1.0, 1.0);
  double bds[6];
  bds[0] = upperRight->GetComputedDisplayValue(view->GetRenderer())[0] - 50;
  bds[1] = bds[0] + 50;
  bds[2] = upperRight->GetComputedDisplayValue(view->GetRenderer())[1] - 50;
  bds[3] = bds[2] + 50;
  bds[4] = bds[5] = 0.0;
  representation->SetPlaceFactor(1);
  representation->PlaceWidget(bds);
  MyClass temp;
  buttonWidget->AddObserver(vtkCommand::LeftButtonPressEvent,&temp, &MyClass::KeypressCallbackFunction);
  buttonWidget->On();


  //Finally render the scene and compare the image to a reference image
  view->GetRenderer()->AddActor(textActor);

  view->GetRenderWindow()->SetMultiSamples(0);
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}


