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
#include <vtk-8.1/vtkButtonRepresentation.h>
#include <vtk-8.1/vtkButtonWidget.h>
#include <vtk-8.1/vtkChartXY.h>
#include <vtk-8.1/vtkContextScene.h>
#include <vtk-8.1/vtkContextView.h>
#include <vtk-8.1/vtkDelaunay2D.h>
#include <vtk-8.1/vtkImageActor.h>
#include <vtk-8.1/vtkImageData.h>
#include <vtk-8.1/vtkIntArray.h>
#include <vtk-8.1/vtkPlot.h>
#include <vtk-8.1/vtkPolyDataMapper.h>
#include <vtk-8.1/vtkProperty.h>
#include <vtk-8.1/vtkRectangularButtonSource.h>
#include <vtk-8.1/vtkRenderWindow.h>
#include <vtk-8.1/vtkRenderWindowInteractor.h>
#include <vtk-8.1/vtkRenderer.h>
#include <vtk-8.1/vtkSmartPointer.h>
#include <vtk-8.1/vtkTable.h>
#include <vtk-8.1/vtkTextActor.h>
#include <vtk-8.1/vtkTextProperty.h>
#include <vtk-8.1/vtkTexturedButtonRepresentation2D.h>
#include <vtk-8.1/vtkVersion.h>
#include <vtk-8.1/vtkVertexGlyphFilter.h>

#include <chrono>
#include <random>

#define VTK_CREATE(type, name) vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

void CreateImage(vtkSmartPointer<vtkImageData> image)
{
    // Specify the size of the image data
    image->SetDimensions(10, 10, 1);
    image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
}

class vtkButtonCallback : public vtkCommand
{
  public:
    static vtkButtonCallback *New()
    {
        return new vtkButtonCallback;
    }
    virtual void Execute(vtkObject *caller, unsigned long, void *)
    {
        vtkButtonWidget *buttonWidget = reinterpret_cast<vtkButtonWidget *>(caller);
        std::cout << "Button clicked" << std::endl;

        auto start = std::chrono::steady_clock::now();
        table->RemoveColumn(0);
        table->RemoveColumn(1);
        VTK_CREATE(vtkIntArray, tblIndex);
        tblIndex->SetName("Index");
        table->AddColumn(tblIndex);

        VTK_CREATE(vtkIntArray, arrMonth);
        arrMonth->SetName("Month");
        table->AddColumn(arrMonth);

        table->SetNumberOfRows(500);
        for (std::size_t i = 0; i < 500; ++i)
        {
            table->SetValue(i, 0, i);
            table->SetValue(i, 1, std::rand() % 5);
        }

        auto end  = std::chrono::steady_clock::now();
        auto diff = end - start;
        std::cout << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;
    }
    vtkButtonCallback() : Actor(0), table(0)
    {
    }
    vtkActor *Actor;
    vtkTable *table;
};

// Monthly circulation data
int main(int, char *[])
{
    // Set up a 2D scene, add an XY chart to it
    VTK_CREATE(vtkContextView, view);
    view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
    view->GetRenderWindow()->SetSize(800, 400);
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
    // Add multiple line plots, setting the colors etc
    vtkPlot *line = 0;

    line = chart->AddPlot(vtkChart::BAR);
    line->SetInputData(table, 0, 1);
    line->SetColor(0, 255, 0, 255);

    vtkSmartPointer<vtkTextProperty> textProperty = vtkSmartPointer<vtkTextProperty>::New();
    textProperty->SetFontSize(16);
    textProperty->SetColor(1.0, 0.0, 0.0);
    textProperty->SetJustificationToCentered();

    vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetInput("TEST");
    textActor->SetPosition(400.0, 380.0);
    //  textActor->SetPosition2 ( 100, 100 );
    textActor->SetTextProperty(textProperty);

    VTK_CREATE(vtkButtonWidget, buttonWidget);
    VTK_CREATE(vtkTexturedButtonRepresentation2D, representation);
    VTK_CREATE(vtkImageData, image);
    VTK_CREATE(vtkImageData, image2);
    CreateImage(image);
    CreateImage(image2);
    representation->SetNumberOfStates(2);
    representation->SetButtonTexture(0, image);
    representation->SetButtonTexture(1, image2);
    buttonWidget->SetInteractor(view->GetInteractor());
    buttonWidget->SetRepresentation(representation);
    vtkSmartPointer<vtkCoordinate> upperRight = vtkSmartPointer<vtkCoordinate>::New();
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
    vtkSmartPointer<vtkButtonCallback> callback = vtkSmartPointer<vtkButtonCallback>::New();
    buttonWidget->AddObserver(vtkCommand::StateChangedEvent, callback);
    callback->table = table;
    buttonWidget->On();

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

    unsigned int gridSize = 1000;
    for (unsigned int x = 0; x < gridSize; x++)
    {
        for (unsigned int y = 0; y < gridSize; y++)
        {
            points->InsertNextPoint(x, y, vtkMath::Random(0.0, 3.0));
        }
    }

    // Add the grid points to a polydata object
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);

    vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    glyphFilter->SetInputData(polydata);
    glyphFilter->Update();

    // Create a mapper and actor
    VTK_CREATE(vtkPolyDataMapper, pointsMapper);
    pointsMapper->SetInputConnection(glyphFilter->GetOutputPort());

    VTK_CREATE(vtkActor, pointsActor);
    pointsActor->SetMapper(pointsMapper);
    pointsActor->GetProperty()->SetPointSize(3.0);
    pointsActor->GetProperty()->SetColor(1.0, 0.0, 0.0);

    view->GetRenderer()->AddActor(pointsActor);
    view->GetRenderer()->AddActor(textActor);
    view->GetRenderWindow()->SetMultiSamples(0);
    view->GetInteractor()->Initialize();
    view->GetRenderer()->Render();

    view->GetInteractor()->Start();

    return EXIT_SUCCESS;
}
