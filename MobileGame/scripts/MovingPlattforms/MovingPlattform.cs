using UnityEngine;

public class MovingPlattform : MonoBehaviour
{
    [SerializeField] private Transform plattformTransform;
    [SerializeField] float _xPos = 0f;
    [SerializeField] float _zPosOffset = 0f;
    [Header("xPos random between -lanewidth > 0 > lanewidth if not fixed")]
    [SerializeField] bool _isRandomLane = false;

    [SerializeField] private float _speed = 1.5f;
    float _laneWidth = 1.5f;

    private void Start()
    {
        if (_isRandomLane == true)
        {
            _xPos = (UnityEngine.Random.Range(0, 3) - 1) * _laneWidth;
        }
    }

    public void Initialize(float xPos, float zPos, bool isRandomLane = false)
    {
        _zPosOffset = zPos;
        _isRandomLane = isRandomLane;
        if (_isRandomLane == true)
        {
            _xPos = (UnityEngine.Random.Range(0, 3) - 1) * _laneWidth;
            return;
        }
        _xPos = xPos;
    }

    private void Update()
    {
        plattformTransform.position = new Vector3(_xPos ,0f ,transform.position.z * _speed + _zPosOffset);
    }
}
