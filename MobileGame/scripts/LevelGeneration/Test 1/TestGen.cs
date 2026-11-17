using UnityEngine;

public class TestGen : MonoBehaviour
{
    [SerializeField] ObstaclePattern _obstaclePattern1;
    [SerializeField] ObstaclePattern _obstaclePattern2;

    float _currentPosition = 0;
    float _laneSize = 1.5f;

    private void Awake()
    {
        Application.targetFrameRate = 60;
    }

    void Start()
    {
        ProcessPattern(_obstaclePattern1);
        ProcessPattern(_obstaclePattern2);
        ProcessPattern(_obstaclePattern1);

    }

    private void ProcessPattern(ObstaclePattern obstaclePattern)
    {
        
        for (int i = 0; i < obstaclePattern.obstacleRows.Count; i++)
        {
            ObstacleRow row = obstaclePattern.obstacleRows[i];


            float maxSpaceNeeded = 1;
            if (row.lane1 != null)
            {
                GameObject go = Instantiate(row.lane1.obstaclePrefab);
                go.transform.position = Vector3.forward * _currentPosition - Vector3.right * _laneSize;

                float spaceNeeded = row.lane1.spaceNeeded;
                maxSpaceNeeded = maxSpaceNeeded < spaceNeeded ? spaceNeeded : maxSpaceNeeded;
            }
            if (row.lane2 != null)
            {
                GameObject go = Instantiate(row.lane2.obstaclePrefab);
                go.transform.position = Vector3.forward * _currentPosition;

                float spaceNeeded = row.lane2.spaceNeeded;
                maxSpaceNeeded = maxSpaceNeeded < spaceNeeded ? spaceNeeded : maxSpaceNeeded;
            }
            if (row.lane3 != null)
            {
                GameObject go = Instantiate(row.lane3.obstaclePrefab);
                go.transform.position = Vector3.forward * _currentPosition + Vector3.right * _laneSize;

                float spaceNeeded = row.lane3.spaceNeeded;
                maxSpaceNeeded = maxSpaceNeeded < spaceNeeded ? spaceNeeded : maxSpaceNeeded;
            }
            
            _currentPosition += maxSpaceNeeded;
        }
    }


}
