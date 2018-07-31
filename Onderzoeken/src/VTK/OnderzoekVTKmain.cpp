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

#include <random>
#include <chrono>

#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()


// Monthly circulation data
int main(int, char*[] )
{
  // Set up a 2D scene, add an XY chart to it
  VTK_CREATE(vtkContextView, view);
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  view->GetRenderWindow()->SetSize(400, 300);
  VTK_CREATE(vtkChartXY, chart);
  view->GetScene()->AddItem(chart);

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

  //Finally render the scene and compare the image to a reference image
  view->GetRenderWindow()->SetMultiSamples(0);
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}


