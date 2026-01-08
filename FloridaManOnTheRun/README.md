<!-- Formating Docs -->
<!-- https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax -->
# *Florida Man On The Run*

<img src="Images\FloridaMan-Lazy_Crock_CROP.jpg" width="100%"/>

[Florida Man On The Run - Itch Link](https://yrgo-game-creator.itch.io/florida-man-on-the-run)

Worked as **Gameplay Programmer / Project Manager / Producer**  
*2025 December - 2026 Januari*

Florida Man on The Run is a 2D side scrolling extreme sports game in the vein of Tony Hawk / Dave Mirra BMX.\ 
It's an Arcade Sport Games, with graphics inspired by year 2000 cartoons.\
I developed and pitched the game idea during a game design course and it won by external jury to be worked on as a group.\
We made the game during 8 weeks with 3 programmers and 4 artists.

## Gameplay

<table>
  <tr>
    <td ><img src="Images\template-GIF.gif"/></td>
  </tr>
</table>

**During the project**
- Game Concept and Pitch
- Player Controller
- Camera
- Parralax
- Level design Art Tool
- Project Management
- Sound Manager implementattion

**Some of the implementations involves:**
- Input Management / Combo System / UI
- Player interactions with external triggers
- Talking and adding functionality to Game managers, UI manager
- Scriptable Objects
- State Machine
- Singeltons

**Other tasks & Improvements I made:**  
- Animation controller using .crossfade, Integration of animations, Some Animation work
- Particle system integration
- Tooling using Scriptable Objects for placement of prefabs in levels

---

## Tools Created 

**Art Tool for level design**

<table>
  <tr>
  <!-- Insert Gif showing the tool -->
    <td ><img src="Images\template-GIF.gif"/></td>
  </tr>
</table>

A tool for quickly placing random perfab assets along a line.\
Each Art Prefab are referenced in a Scriptable Object containting the perfered distance it needs to fit, and a offset value.\
[ArtObjectSO.cs](/Script/ArtTool/ArtObjectSO.cs)

That is then put in another Scriptable Object that contains multiple Arrays of the different type of Art Assets.\
[ArtObjectCollectionsSO.cs](/Script/ArtTool/ArtObjectCollectionSO.cs)

The placement is handled by the ArtTool script.\
[ArtTool.cs](/Script/ArtTool/ArtTool.cs)

And the interface for the user in an Editor Script.\
[ArtToolEditorScript.cs](/Script/Editor/ArtTool/ArtToolEditorScript.cs)


Click the dropdown arrow or click the link below to see the code

<details>
<summary>ArtToolEditorScript.cs <- click to show code - </summary>

```csharp
using System;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

[CustomEditor(typeof(ArtTool))]
public class ArtToolEditorScript : Editor
{

    public override void OnInspectorGUI()
    {
        //LevelBuilderTool levelBuilderTool = (LevelBuilderTool)target;
        ArtTool artTool = (ArtTool)target;

        artTool.CurrentPrefab = (GameObject)EditorGUILayout.ObjectField("Prefab", artTool.CurrentPrefab, typeof(GameObject), false);
        artTool.Spacing = (float)EditorGUILayout.FloatField("Spacing", artTool.Spacing);
        artTool.YOffsett = (float)EditorGUILayout.FloatField("Y Offset", artTool.YOffsett);
        artTool.SpacingCount = (int)EditorGUILayout.IntField("Spacing Count", artTool.SpacingCount);
        artTool.ParentToSpline = (bool)EditorGUILayout.Toggle("Parent To Spline", artTool.ParentToSpline);


        if (GUILayout.Button("Place Prefab Asset"))
        {
            //levelBuilderTool.RemoveChildren();
            Debug.Log("Pressing - Place Assets");
            artTool.PlaceChoosenPrefab();
        }

        artTool.ArtObjectCollectionSO = (ArtObjectCollectionSO)EditorGUILayout.ObjectField("Prefab", artTool.ArtObjectCollectionSO, typeof(ArtObjectCollectionSO), false);

        if (GUILayout.Button("Place Default Base"))
        {
            artTool.artObjectType = ArtTool.ArtObjectType.FG_house;
            artTool.PlaceArtCollectionSO();
            artTool.artObjectType = ArtTool.ArtObjectType.FG_Sidewalk;
            artTool.PlaceArtCollectionSO();
            artTool.artObjectType = ArtTool.ArtObjectType.FG_Road;
            artTool.PlaceArtCollectionSO();
        }

        if (GUILayout.Button("Place House"))
        {
            artTool.artObjectType = ArtTool.ArtObjectType.FG_house;
            artTool.PlaceArtCollectionSO();
        }
        // ...
        // Shortend list of buttons
        // ...
        if (GUILayout.Button("Place L3 BG"))
        {
            artTool.artObjectType = ArtTool.ArtObjectType.L3_house;
            artTool.PlaceArtCollectionSO();
        }
        if (GUILayout.Button("Place BG Tree"))
        {
            artTool.artObjectType = ArtTool.ArtObjectType.BG_Tree;
            artTool.PlaceArtCollectionSO();
        }
    }
}

```

</details>

<details>
<summary>ArtTool.cs <- click to show code</summary>

 ```csharp

using System;
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

```

</details>

---

## Game Design

<table>
  <tr>
    <td ><img src="Images\template-GIF.gif"/></td>
  </tr>
</table>

I worked on the core concept of the game.\
The player controller.\
The look and feel of the game together with the artists.\
Sound and UI elements.

###  Tools I worked with:  
[Unity](https://www.unity.com/): Game engine