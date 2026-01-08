using UnityEngine;

[CreateAssetMenu(fileName = "ArtCollectionSO", menuName = "ArtTool/ArtCollectionSO")]

public class ArtObjectCollectionSO : ScriptableObject
{
    public ArtObjectSO[] ForegroundHousesCollection;
    public ArtObjectSO[] L1HousesCollection;
    public ArtObjectSO[] L2HousesCollection;
    public ArtObjectSO[] L3HousesCollection;
    public ArtObjectSO[] RoadCollection;
    public ArtObjectSO[] ScaffoldCollection;
    public ArtObjectSO[] SidewalkCollection;
    public ArtObjectSO[] TreeCollection;
    public ArtObjectSO[] SmallObjectsCollection;
    public ArtObjectSO[] BGTreeCollection;
}