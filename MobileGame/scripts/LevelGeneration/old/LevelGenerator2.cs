using UnityEngine;
using UnityEngine.InputSystem;

// Old level generator code --- OBS
public class LevelGenerator2 : MonoBehaviour
{
    public GameObject[] _baseLevelPrefab;

    private Transform[] _levelSections = new Transform[20]; // Allocate 20 places
    private int _maxSections = 20;
    private int _initialLevelSectionsToCreate = 15;
    private int _levelSectionsCount = 0;
    private int _startIndexSection = 0;

    private Vector3 _startPos = Vector3.zero;
    private GameObject _goTemp;

    [SerializeField] private GameObject[] _dangerPropsPrefabs;
    [SerializeField] private GameObject _coinPrefab;
    private float _laneWidth = 1.5f;


    private void AddNewSection()
    {
        _goTemp = Instantiate(GetValidSpawnPrefab(0), GetSpawnPositionFromLast(), Quaternion.identity);

        int lastIndex = GetNextIndex(GetLastIndex());
        //print($"last index = {lastIndex}");

        _levelSections[lastIndex] = _goTemp.transform;
        _levelSectionsCount++;

        PlaceDangerProps(_levelSections[lastIndex]);
        PlaceCoins(_levelSections[lastIndex]);
    }

    private void PlaceDangerProps(Transform parentPiece)
    {
        for (int i = 0; i < 1; i++)
        {
            Vector3 placementPos = new Vector3(Mathf.Round(UnityEngine.Random.Range(-1.45f, 1.45f)) * _laneWidth,
                                                0f, 0f);
            //Mathf.Round(UnityEngine.Random.Range(-5f, 5f)));

            int pickedDanger = UnityEngine.Random.Range(0, _dangerPropsPrefabs.Length);
            Instantiate(_dangerPropsPrefabs[pickedDanger], placementPos + parentPiece.position, Quaternion.identity, parentPiece);
        }
    }

    private void PlaceCoins(Transform parentPiece)
    {
        if (UnityEngine.Random.Range(0, 10) > 6) { return; }

        int lane = UnityEngine.Random.Range(-1, 2);
        int startValue = 0;
        int endValue = 10;

        for (int i = startValue; i < endValue; i += 3)
        {
            // Random chance to switch lanes
            if (UnityEngine.Random.Range(0, 10) > 8)
            {
                if (lane == -1) { lane++; }
                else if (lane == 1) { lane--; }
                else { lane = UnityEngine.Random.Range(-1, 2); }
            }
            Instantiate(_coinPrefab, new Vector3(lane * _laneWidth, 1f, i) + parentPiece.position, Quaternion.identity, parentPiece);
        }
    }

    public void InitLevel()
    {
        _levelSections = new Transform[_maxSections]; // Allocate 
        _levelSectionsCount = 0;
        _startIndexSection = 0;

        _startPos = Vector3.zero;

        // Create base Level
        GameObject goTemp;
        for (int i = 0; i < _initialLevelSectionsToCreate; i++)
        {
            goTemp = Instantiate(GetValidSpawnPrefab(0), _startPos, Quaternion.identity);
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
                Destroy(_levelSections[i].gameObject);
            }
            _levelSections[i] = null;
        }
        //InitLevel();
    }

    private GameObject GetValidSpawnPrefab(int currentIndex)
    {
        // TODO Logic for valid choices if index is
        int index = UnityEngine.Random.Range(0, _baseLevelPrefab.Length);

        return _baseLevelPrefab[index];
    }

    private Vector3 GetSpawnPositionFromLast()
    {
        Transform lastInArray = _levelSections[GetLastIndex()];
        Vector3 newPostion = new Vector3(lastInArray.position.x, lastInArray.position.y, lastInArray.position.z + 10f);
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

            if (_levelSections[index].position.z < -10f)
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
            Destroy(temp);
            _levelSectionsCount--;
            _levelSections[_startIndexSection] = null;
            _startIndexSection = GetNextIndex(_startIndexSection);

            // Add new section
            AddNewSection();
        }
    }
}
