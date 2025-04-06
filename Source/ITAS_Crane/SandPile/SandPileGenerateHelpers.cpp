// CustomDynamicMesh.cpp
#include "SandPileGenerateHelpers.h"


void USandPileGenerateHelpers::ProcessPointsAndUpdateMesh(TArray<FVector> AccumulatedPoints, UDynamicMesh* Mesh)
{
	// Порог для определения близости точек (в сантиметрах)
	float Threshold = 10.0f;
	float ThresholdSquared = Threshold * Threshold;

	// Получаем текущий меш
	FDynamicMesh3* Mesh3 = Mesh->GetMeshPtr();
	if (!Mesh3) return;

	// Массив вершин для работы
	TArray<FVector> Vertices;
	TMap<int32, int32> VertexIDToIndex; // Связь между ID вершины в меше и индексом в Vertices

	// Заполняем существующие вершины из меша
	for (int32 VID : Mesh3->VertexIndicesItr())
	{
		FVector Vertex = Mesh3->GetVertex(VID);
		VertexIDToIndex.Add(VID, Vertices.Num());
		Vertices.Add(Vertex);
	}

	// Обрабатываем накопленные точки
	for (const FVector& Point : AccumulatedPoints)
	{
		FVector2D Point2D(Point.X, Point.Y); // Проекция в плоскость XY
		bool bFoundCloseVertex = false;
		float MinDistSquared = TNumericLimits<float>::Max();
		int32 ClosestIndex = -1;

		// Проверяем расстояние до существующих вершин
		for (int32 i = 0; i < Vertices.Num(); ++i)
		{
			FVector2D Vertex2D(Vertices[i].X, Vertices[i].Y);
			float DistSquared = FVector2D::DistSquared(Point2D, Vertex2D);
			if (DistSquared < ThresholdSquared && DistSquared < MinDistSquared)
			{
				MinDistSquared = DistSquared;
				ClosestIndex = i;
				bFoundCloseVertex = true;
			}
		}

		if (bFoundCloseVertex)
		{
			// Если точка близко к существующей вершине, поднимаем её на 10 см
			Vertices[ClosestIndex].Z += 10.0f;
		}
		else
		{
			// Если нет близких вершин, добавляем новую точку на высоте 10 см
			FVector NewVertex(Point.X, Point.Y, 10.0f);
			Vertices.Add(NewVertex);
		}
	}

	// Очищаем текущий меш
	Mesh3->Clear();

	// Добавляем обновленные вершины в меш
	TArray<int32> VertexIDs;
	for (const FVector& Vertex : Vertices)
	{
		int32 NewVID = Mesh3->AppendVertex(Vertex);
		VertexIDs.Add(NewVID);
	}

	// Упрощенная триангуляция (последовательная)
	for (int32 i = 2; i < VertexIDs.Num(); i += 3)
	{
		if (i < VertexIDs.Num())
		{
			int32 VID0 = VertexIDs[i - 2];
			int32 VID1 = VertexIDs[i - 1];
			int32 VID2 = VertexIDs[i];
			Mesh3->AppendTriangle(VID0, VID1, VID2);
		}
	}

	// После обновления можно очистить накопленные точки, если не требуется их сохранять
	AccumulatedPoints.Empty();
}
