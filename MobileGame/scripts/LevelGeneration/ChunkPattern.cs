using UnityEngine;

[CreateAssetMenu(fileName ="LevelChunk", menuName = "LevelGen/LevelChunk")]
public class ChunkPattern : ScriptableObject
{
    [Header("When using Random chunks")]
    public bool useRandomChunks = false;
    public int minChunks = 3;
    public int maxChunks = 6;

    [Header("Chunk Level Data")]
    //public ChunkData StartData;
    public ChunkData[] ChunkData;
    //public ChunkData EndData;

    public int GetChunkDataLenght()
    {
        if (useRandomChunks)
        {
            int index = Random.Range(minChunks, maxChunks);
            return index;
        }

        return ChunkData.Length;
    }

    public ChunkData SpawnChunk(int index)
    {
        if (useRandomChunks)
        {
            index = Random.Range(0, ChunkData.Length);
            //return Instantiate(ChunkData[index]);
            return ObjectPoolManager.SpawnObject(ChunkData[index], Vector3.forward * 100f, Quaternion.identity);
        }
        return ObjectPoolManager.SpawnObject(ChunkData[index], Vector3.forward * 100f, Quaternion.identity);

        //return Instantiate(ChunkData[index]);
    }
}
