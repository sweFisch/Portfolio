using UnityEngine;

public enum EObstacleType
{
    Plattform1,
    Plattform2,
    Plattform1Movable,
    Plattform2Movable,
    Plattform3,
    PlattformRoof,
    LowObstacle,
    LowDuckableObstacle,
    HighObstacle,
    HighDuckableObstacle,
}

[CreateAssetMenu(fileName ="Obstacle", menuName ="LevelGen/Obstacle")]
public class Obstacle : ScriptableObject
{
    public GameObject obstaclePrefab;
    [Tooltip("1 = jumpheight")]
    public int height = 2;
    [Tooltip("Can the player duck under")]
    public int canDuck;
    public int spaceNeeded = 1;
    // coin placement ?
    public bool spawnCoins;
    public bool spawnCoinArc;
}
