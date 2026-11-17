using UnityEngine;

/// <summary>
/// It has a state Not a Traditional State Machine
/// It can spawn a Start Pattern for a level
/// Then change between Tunnel or Normal pattterns at the moment, but is extendable
/// </summary>
public class LevelStateMachine : MonoBehaviour
{
    public enum SpawnerState
    {
        StartBase,
        NormalLevel,
        Tunnel,
        //TunnelMoving,
        //MovingPlattorms,
    }

    public SpawnerState _state;

    public ChunkPattern StartPattern;
    public ChunkPattern[] NormalLevelPattern;
    public ChunkPattern[] TunnelPattern;

    public ChunkPattern _currentPattern;
    public int patternIndex = 0; // how many times have we called the pattern

    private void Awake()
    {
        RestartLevel();
    }

    public void RestartLevel()
    {
        _state = SpawnerState.StartBase;
        _currentPattern = StartPattern;
        patternIndex = 0;
    }

    public GameObject GetNextChunk()
    {
        if (_currentPattern.GetChunkDataLenght() - 1  < patternIndex)
        {
            //Debug.Log("-- index out of range --");
            //Debug.Log("-- Change State or Pattern and reset patternIndex --");
            SwitchStateRandom();
            patternIndex = 0;
            //Debug.Log($"Switch Pattern to {_state.ToString()} , pattern index = {patternIndex}");
        }

        ChunkData levelChunk = _currentPattern.SpawnChunk(patternIndex);
        levelChunk.SetupChunk();
        
        patternIndex++;

        return levelChunk.gameObject;
    }

    public void SwitchStateRandom()
    {
        int state = Random.Range(0, 100);
        if (state < 30)
        {
            this._state = SpawnerState.Tunnel;
        }
        else 
        {
            this._state = SpawnerState.NormalLevel;
        }
        PickPattern();
        patternIndex = 0;
    }

    public void PickPattern()
    {
        int index = 0;
        switch (_state)
        {
            case SpawnerState.StartBase:
                _currentPattern = StartPattern;
                break;
            case SpawnerState.NormalLevel:
                index = Random.Range(0,NormalLevelPattern.Length);
                _currentPattern = NormalLevelPattern[index];
                break;
            case SpawnerState.Tunnel:
                index = Random.Range(0, TunnelPattern.Length);
                _currentPattern = TunnelPattern[index];
                break;
            default:
                Debug.LogWarning("Pattern selection case does not exist?");
                _currentPattern = NormalLevelPattern[index];
                break;
        }
    }
}
