using System.Collections.Generic;
using System;
using UnityEngine;

[System.Serializable]
public class DangerData
{
    public Vector3 pos;
    public EDangerProp dangerPropType;
    public int spawnChance = 100;
}

[System.Serializable]
public class PowerUpPlacementData
{
    public Vector3 pos;
    public EPowerUp powerUpType;
    public int spawnChance = 100;
}

[System.Serializable]
public enum CoinType
{
    Single,
    Line,
    Arc
}

[System.Serializable]
public class CoinPlacementData
{
    public Vector3 pos;
    public CoinType coinType;
    public int spawnChance = 100;
}



/// <summary>
/// Contains a Data Array of placement, type , and spawnChance - that chunkData uses to load danger placements.
///  Improvements -> ? Breaking this out to an SO as well and separate the prefab look data and the danger placement data
///  Or design danger data with this script that then can be loaded in different configs?
/// </summary>
public class ItemPlacemnetData : MonoBehaviour 
{
    public DangerData[] dangerDataArray;
    public PowerUpPlacementData[] powerUpDataArray;
    public CoinPlacementData[] coinPlacementDataArray;

    private void OnDrawGizmosSelected()
    {
        DrawGizmoDangerProps();

        DrawGizmoPowerUpPlacement();

        DrawGizmoCoinPlacement();
    }


    private void DrawGizmoDangerProps()
    {
        if (dangerDataArray == null) { return; }

        // Draw Danger Placement
        Gizmos.color = Color.red;
        foreach (var data in dangerDataArray)
        {
            Vector3 pos = data.pos + transform.position;
            Vector3 size = Vector3.one;

            switch (data.dangerPropType)
            {
                case EDangerProp.Low:
                    break;
                case EDangerProp.LowDuck:
                    break;
                case EDangerProp.HighDuck:
                    size = Vector3.one + Vector3.up;
                    break;
                case EDangerProp.High:
                    size = Vector3.one + Vector3.up;
                    break;
                case EDangerProp.LowPlattform:
                    size = new Vector3(1f, 1.1f, 7f);
                    break;
                case EDangerProp.HighPlattform:
                    size = new Vector3(1f, 2.2f, 7f);
                    break;
                case EDangerProp.HighMoving:
                    size = new Vector3(1f, 2.2f, 7f);
                    break;
                case EDangerProp.LowMoving:
                    size = new Vector3(1f, 1.1f, 7f);
                    break;
            }
            pos += new Vector3(0f, size.y * 0.5f, 0f);
            Gizmos.DrawWireCube(pos, size);
        }
    }

    private void DrawGizmoPowerUpPlacement()
    {
        if (powerUpDataArray == null) { return; }

        // Draw powerup Placement
        Gizmos.color = Color.green;
        foreach (var data in powerUpDataArray)
        {
            Vector3 pos = data.pos + transform.position;
            Vector3 size = Vector3.one;

            //switch (data.powerUpType)
            //{
            //    case EPowerUp.PowerJump:
            //        break;
            //    case EPowerUp.Sword:
            //        break;
            //    case EPowerUp.Granade:
            //        break;
            //    case EPowerUp.Bow:
            //        break;
            //    case EPowerUp.Sheild:
            //        break;
            //    case EPowerUp.Flying:
            //        break;
            //    case EPowerUp.CoinMagnet:
            //        break;
            //    case EPowerUp.DoubleCoins:
            //        break;
            //    default:
            //        break;
            //}
            pos += new Vector3(0f, size.y * 0.5f, 0f);
            Gizmos.DrawWireCube(pos, size);
        }
    }

    public Vector3[] GetCoinPlacementPosData()
    {
        if (coinPlacementDataArray == null) { return null; }

        List<Vector3> posList = new List<Vector3>();
        float heighOffset = 0.5f;

        foreach (var data in coinPlacementDataArray)
        {
            Vector3 pos = data.pos + transform.position + Vector3.up * heighOffset;

            Vector3[] tempPosArray;

            switch (data.coinType)
            {
                case CoinType.Single:
                    posList.Add(pos);
                    break;
                case CoinType.Line:
                    tempPosArray = new Vector3[10];
                    for (int i = 0; i < 10; i += 2)
                    {
                        //tempPosArray[i] = new Vector3(pos.x, pos.y, pos.z + i);
                        posList.Add(new Vector3(pos.x, pos.y, pos.z + i));
                    }
                    break;
                case CoinType.Arc:
                    tempPosArray = new Vector3[5];
                    //tempPosArray[0] = pos;
                    posList.Add(new Vector3(pos.x, pos.y, pos.z));

                    for (int i = 1; i < 10; i += 2)
                    {
                        float yArc = -((i - 5f) * (i - 5f)) * 0.10f + 2; // checked with graph calc - https://www.desmos.com/calculator
                        //tempPosArray[i] = new Vector3(pos.x, pos.y + yArc, pos.z + i);
                        posList.Add(new Vector3(pos.x, pos.y + yArc, pos.z + (i))); // 
                    }
                    break;
            }
        }
        return posList.ToArray();
    }

    private void DrawGizmoCoinPlacement()
    {
        if(coinPlacementDataArray == null) { return; }
        Gizmos.color = Color.yellow;

        Vector3[] coinPosArray = GetCoinPlacementPosData();

        foreach (var posA in coinPosArray)
        {
            Vector3 pos = posA;
            Vector3 size = Vector3.one * 0.3f;

            //pos += new Vector3(0f, size.y * 0.5f + 0.5f, 0f);
            //bool drawSingle = true;
            //Vector3[] tempPosArray;

            //switch (data.coinType)
            //{
            //    case CoinType.Single:

            //        break;
            //    case CoinType.Line:
            //        drawSingle = false;
            //        tempPosArray = new Vector3[10];
            //        for (int i = 0; i < 10; i++)
            //        {
            //            tempPosArray[i] = new Vector3(pos.x,pos.y,pos.z + i);
            //        }
            //        break;
            //    case CoinType.Arc:
            //        drawSingle = false;
            //        tempPosArray = new Vector3[10];
            //        for (int i = 0; i < 10; i++)
            //        {
            //            float yArc = -((pos.x-5f) * (pos.x - 5f) * 0.33f) + 1; // checked with graph calc - https://www.desmos.com/calculator
            //            tempPosArray[i] = new Vector3(pos.x, pos.y + yArc, pos.z + i);
            //        }
            //        break;
            //}
            Gizmos.DrawWireCube(pos, size);
        }
    }
}
