using UnityEngine;
using UnityEngine.InputSystem;

public class LevelGenerator : MonoBehaviour
{
    public LevelStateMachine _levelStateMachine;

    private Transform[] _levelSections = new Transform[20]; // Allocate 20 places
    private int _maxSections = 20;
    private int _initialLevelSectionsToCreate = 15;
    private int _levelSectionsCount = 0;
    private int _startIndexSection = 0;

    private Vector3 _startPos = Vector3.zero;
    private GameObject _goTemp;

    private float _laneWidth = 1.5f;


    private void AddNewSection()
    {
        Vector3 spawnPos = GetSpawnPositionFromLast();
        _goTemp = _levelStateMachine.GetNextChunk();
        _goTemp.transform.position = spawnPos;

        int lastIndex = GetNextIndex(GetLastIndex());
        //print($"last index = {lastIndex}");

        _levelSections[lastIndex] = _goTemp.transform;
        _levelSectionsCount++;
    }

    public void InitLevel()
    {
        ResetLevel();
        _levelStateMachine.RestartLevel();

        _levelSections = new Transform[_maxSections]; // Allocate 
        _levelSectionsCount = 0;
        _startIndexSection = 0;

        _startPos = Vector3.zero;

        // Create base Level
        GameObject goTemp;
        for (int i = 0; i < _initialLevelSectionsToCreate; i++)
        {
            //goTemp = Instantiate(GetValidSpawnPrefab(0), _startPos, Quaternion.identity);
            Vector3 spawnPos = _startPos;
            goTemp = _levelStateMachine.GetNextChunk();
            goTemp.transform.position = spawnPos;

            _levelSections[i] = goTemp.transform;
            _levelSections[i].position = _startPos;

            float sectionSize = 10f + _startPos.z;

            _startPos = new Vector3(0f, 0f, sectionSize);
            _levelSectionsCount++;
        }
    }

    public void ResetLevel()
    {
        for (int i = 0; i < _maxSections; i++)
        {
            if (_levelSections[i] != null)
            {
                //Destroy(_levelSections[i].gameObject);
                GameObject temp = _levelSections[i].gameObject;
                temp.GetComponent<ChunkData>()?.DestroyLevelChunk();
            }
            _levelSections[i] = null;
        }
        //InitLevel();
    }

    private Vector3 GetSpawnPositionFromLast()
    {
        Transform lastInArray = _levelSections[GetLastIndex()];
        Vector3 newPostion = new Vector3(lastInArray.position.x, lastInArray.position.y, lastInArray.position.z + 10f); // chunk Level lenght (size in Z)
        return newPostion;
    }


    private int GetNextIndex(int index)
    {
        index++;
        if (index >= _maxSections)
        {
            return 0;
        }
        return index;
    }
    private int GetIndex(int index)
    {
        return index % _maxSections;
    }

    private int GetLastIndex()
    {
        int index = _startIndexSection + _levelSectionsCount - 1;
        return index % _maxSections;
    }

    public void MoveLevel(Vector3 gameSpeedVector)
    {
        bool destroyGO = false;
        int destroyIndex = 0;

        for (int i = 0; i < _levelSectionsCount; i++)
        {
            int index = GetIndex(_startIndexSection + i);
            //print($"moving index {index}");

            if (_levelSections[index] == null) { continue; }

            _levelSections[index].Translate(gameSpeedVector);

            if (_levelSections[index].position.z < -15f)
            {
                destroyGO = true;
                destroyIndex = index;
                if (destroyIndex != _startIndexSection)
                {
                    Debug.LogWarning("-- Something went wrong! In deletion --");
                }
            }
        }
        if (destroyGO) // Once per frame
        {
            GameObject temp = _levelSections[destroyIndex].gameObject;
            temp.GetComponent<ChunkData>()?.DestroyLevelChunk();

            //Destroy(temp);
            _levelSectionsCount--;
            _levelSections[_startIndexSection] = null;
            _startIndexSection = GetNextIndex(_startIndexSection);

            // Add new section
            AddNewSection();
        }
    }
}
