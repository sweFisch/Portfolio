using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class ObstacleRow
{
    public Obstacle lane1;
    public Obstacle lane2;
    public Obstacle lane3;
}


[CreateAssetMenu(fileName = "ObstaclePattern", menuName = "LevelGen/ObstaclePattern")]
public class ObstaclePattern : ScriptableObject
{
    public List<ObstacleRow> obstacleRows;
}