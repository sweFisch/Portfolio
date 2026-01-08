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
        if (GUILayout.Button("Place Scaffold"))
        {
            artTool.artObjectType = ArtTool.ArtObjectType.FG_Scaffold;
            artTool.PlaceArtCollectionSO();
        }
        if (GUILayout.Button("Place Sidewalk"))
        {
            artTool.artObjectType = ArtTool.ArtObjectType.FG_Sidewalk;
            artTool.PlaceArtCollectionSO();
        }
        if (GUILayout.Button("Place Road"))
        {
            artTool.artObjectType = ArtTool.ArtObjectType.FG_Road;
            artTool.PlaceArtCollectionSO();
        }
        if (GUILayout.Button("Place Tree"))
        {
            artTool.artObjectType = ArtTool.ArtObjectType.FG_Tree;
            artTool.PlaceArtCollectionSO();
        }
        if (GUILayout.Button("Place Small Objects"))
        {
            artTool.artObjectType = ArtTool.ArtObjectType.SmallObjects;
            artTool.PlaceArtCollectionSO();
        }
        if (GUILayout.Button("Place L1 BG"))
        {
            artTool.artObjectType = ArtTool.ArtObjectType.L1_house;
            artTool.PlaceArtCollectionSO();
        }
        if (GUILayout.Button("Place L2 BG"))
        {
            artTool.artObjectType = ArtTool.ArtObjectType.L2_house;
            artTool.PlaceArtCollectionSO();
        }
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