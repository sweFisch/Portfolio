using System;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Splines;

public class ArtTool : MonoBehaviour
{
    public enum ArtObjectType
    {
        FG_house,
        L1_house,
        L2_house,
        L3_house,
        FG_Sidewalk,
        FG_Road,
        FG_Scaffold,
        FG_Tree,
        SmallObjects,
        BG_Tree,
    }
    public ArtObjectType artObjectType;

    public ArtObjectCollectionSO ArtObjectCollectionSO;
    public GameObject CurrentPrefab;
    public float Spacing = 10f;
    public float YOffsett = 0f;
    public int SpacingCount = 0;
    public bool ParentToSpline = true;

    private SplineContainer _splineContainer;
    Vector3 startPos, endPos;

    Transform _createdObjectsParent;

    public void CalculateStartAndEnd()
    {
        _splineContainer = GetComponent<SplineContainer>();
        startPos = (_splineContainer.Spline.EvaluatePosition(0f));
        endPos = (_splineContainer.Spline.EvaluatePosition(1f));
    }

    public GameObject CreatePrefabAsset(GameObject prefab)
    {
        GameObject currentCopy;
#if UNITY_EDITOR
        currentCopy = (GameObject)UnityEditor.PrefabUtility.InstantiatePrefab(prefab, _createdObjectsParent);
#else
        currentCopy = Instantiate(prefab, _createdObjectsParent);
#endif
        return currentCopy;
    }

    public void PlaceChoosenPrefab()
    {
        CalculateStartAndEnd();

        if (CurrentPrefab!= null)
        {
            GameObject currentCopy;

            float totalDistance = endPos.x - startPos.x;
            if(Spacing <= 0f && SpacingCount <= 0)
            {
                Debug.LogWarning("Spacing must be positive over 0f");
                return;
            }
            int placeCount = Mathf.RoundToInt(totalDistance/Spacing);
            if (SpacingCount > 0)
            {
                placeCount = SpacingCount;
                Spacing = totalDistance/(SpacingCount-1f);
            }
            if (placeCount < 0 || placeCount > 100)
            {
                Debug.LogWarning("Place Count over 100 - Aborting");
                return;
            }

            float currentXOffset = 0f;
            for (int i = 0; i < placeCount; i++)
            {
                currentXOffset = Spacing * i;

                if (ParentToSpline == true)
                {
                    _createdObjectsParent = this.transform;
                }
                else { _createdObjectsParent = null; }

                currentCopy = CreatePrefabAsset(CurrentPrefab);

                currentCopy.transform.position = new Vector3(startPos.x + currentXOffset + transform.position.x, transform.position.y + YOffsett, 0f);

            }
        }
    }

    public void PlaceArtCollectionSO()
    {
        CalculateStartAndEnd();
        CreateOrFindParent(); // Tries to find a child with the enum name and sets it as the parent for the created objects

        GameObject currentCopy;

        float travel = 0f;
        float maxDistance = endPos.x - startPos.x;

        for (int i = 0; i < 100; i++)
        {
            ArtObjectSO currentArtObj = GetArtObjecftFromArtObjectCollection(artObjectType);

            if(currentArtObj == null) { Debug.LogWarning("Missing artObjectType : " + artObjectType.ToString() + " : Breaking out of Loop!" ); break; }

            if (currentArtObj != null) 
            {
                travel += currentArtObj.xSpacing/2f;
                
                currentCopy = CreatePrefabAsset(currentArtObj._artObjectPrefab);
                currentCopy.transform.position = new Vector3(startPos.x + travel + transform.position.x, startPos.y + currentArtObj.yOffset + transform.position.y, 0f);
                
                travel += currentArtObj.xSpacing / 2f;
            }

            if ( travel > maxDistance)
            {
                //print("Break loop");
                break;
            }
        }
    }

    public ArtObjectSO GetArtObjecftFromArtObjectCollection(ArtObjectType type)
    {
        switch (type)
        {
            case ArtObjectType.FG_house:
                return GetRandomObjectFromArtObjectArraySO(ArtObjectCollectionSO.ForegroundHousesCollection);
            case ArtObjectType.L1_house:
                return GetRandomObjectFromArtObjectArraySO(ArtObjectCollectionSO.L1HousesCollection);
            case ArtObjectType.L2_house:
                return GetRandomObjectFromArtObjectArraySO(ArtObjectCollectionSO.L2HousesCollection);
            case ArtObjectType.L3_house:
                return GetRandomObjectFromArtObjectArraySO(ArtObjectCollectionSO.L3HousesCollection);
            case ArtObjectType.FG_Sidewalk:
                return GetRandomObjectFromArtObjectArraySO(ArtObjectCollectionSO.SidewalkCollection);
            case ArtObjectType.FG_Road:
                return GetRandomObjectFromArtObjectArraySO(ArtObjectCollectionSO.RoadCollection);
            case ArtObjectType.FG_Scaffold:
                return GetRandomObjectFromArtObjectArraySO(ArtObjectCollectionSO.ScaffoldCollection);
            case ArtObjectType.FG_Tree:
                return GetRandomObjectFromArtObjectArraySO(ArtObjectCollectionSO.TreeCollection);
            case ArtObjectType.SmallObjects:
                return GetRandomObjectFromArtObjectArraySO(ArtObjectCollectionSO.SmallObjectsCollection);
            case ArtObjectType.BG_Tree:
                return GetRandomObjectFromArtObjectArraySO(ArtObjectCollectionSO.BGTreeCollection);
            default: return null;
        }
    }

    public ArtObjectSO GetRandomObjectFromArtObjectArraySO(ArtObjectSO[] artObjectSOs)
    {
        if(artObjectSOs.Length == 0) 
        {
            Debug.LogWarning("Missing ArtObjectSO in Collection : " + artObjectType.ToString());
            return null; 
        }

        int index = UnityEngine.Random.Range(0, artObjectSOs.Length);
        return artObjectSOs[index];
    }

    public void CreateOrFindParent()
    {
        _createdObjectsParent = transform.Find(artObjectType.ToString() + "_grp");

        if (_createdObjectsParent == null)
        {
            GameObject temp = Instantiate(new GameObject(artObjectType.ToString() + "_grp"),transform);
            temp.name = artObjectType.ToString() + "_grp";
            _createdObjectsParent = temp.transform;
        }
    }
}
