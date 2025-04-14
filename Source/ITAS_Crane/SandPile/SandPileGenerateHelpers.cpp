// CustomDynamicMesh.cpp
#include "SandPileGenerateHelpers.h"

// void UpdateSandPileMesh(UDynamicMesh* DynamicMesh, const TArray<FVector>& WorldPoints)
// {
// 	for (const FVector& WorldPoint : WorldPoints)
// 	{
// 		// Шаг 1: Преобразование в локальное пространство
// 		FVector LocalPoint = DynamicMesh->GetTransform().InverseTransformPosition(WorldPoint);
//
// 		// Шаг 2: Поиск ближайшей вершины
// 		int32 ClosestVertexID = -1;
// 		float MinDistance = 0.1f; // Порог расстояния
// 		for (int32 VertexID = 0; VertexID < DynamicMesh->GetVertexCount(); ++VertexID)
// 		{
// 			FVector VertexPos = DynamicMesh->GetVertex(VertexID);
// 			float Distance = FVector::Distance(LocalPoint, VertexPos);
// 			if (Distance < MinDistance)
// 			{
// 				ClosestVertexID = VertexID;
// 				break;
// 			}
// 		}
//
// 		// Шаг 3 и 4: Добавление или подъем вершины
// 		int32 VertexID;
// 		if (ClosestVertexID == -1)
// 		{
// 			// Новая вершина
// 			VertexID = DynamicMesh->AppendVertex(LocalPoint);
// 		}
// 		else
// 		{
// 			// Существующая вершина
// 			VertexID = ClosestVertexID;
// 		}
// 		FVector CurrentPos = DynamicMesh->GetVertex(VertexID);
// 		FVector NewPos = CurrentPos + FVector(0, 0, 0.1f); // Подъем на 0.1
// 		DynamicMesh->SetVertex(VertexID, NewPos);
//
// 		// Шаг 5: Обновление треугольников (упрощенный пример)
// 		if (ClosestVertexID == -1)
// 		{
// 			// Найти две ближайшие вершины и создать треугольник
// 			// (Логика поиска опущена для краткости)
// 			int32 NearbyVertex1 = 0; // Пример
// 			int32 NearbyVertex2 = 1; // Пример
// 			DynamicMesh->AppendTriangle(VertexID, NearbyVertex1, NearbyVertex2);
// 		}
// 	}
//
// 	// Шаг 6: Обновление меша
// 	DynamicMesh->UpdateMesh();
// }

UDynamicMesh* USandPileGenerateHelpers::ProcessPointsAndUpdateMesh(const TArray<FVector>& AccumulatedPoints, UDynamicMesh* Mesh)
{
	// Сбрасываем меш перед обновлением
	Mesh->Reset();
	
	// Порог для определения близости точек (в сантиметрах)
	float Threshold = 10.0f;
	float ThresholdSquared = Threshold * Threshold;

	// Массив вершин для работы
	TArray<FVector> Vertices;
	TMap<int32, int32> VertexIDToIndex; // Связь между ID вершины в меше и индексом в Vertices

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

	// Получаем текущий меш
	FDynamicMesh3* Mesh3 = Mesh->GetMeshPtr();

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

	return nullptr;
}
