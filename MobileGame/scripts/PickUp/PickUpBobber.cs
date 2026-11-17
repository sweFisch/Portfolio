using UnityEngine;

public class PickUpBobber : MonoBehaviour
{
    [SerializeField] float _heightOffset = 0.5f;
    [Tooltip("Higher values gives a faster bob")]
    [SerializeField] float _speed = 3f;
    Transform _myTransform;
    float _start;
    float _currentHeight;
    float _t;
    float _repeatRange = Mathf.PI * 2; // sin uses radian angles, clamping to one Pi gives a bounce effect

    [SerializeField] bool _bounce = false;

    private void Start()
    {
        _myTransform = transform;
        _start = transform.localPosition.y + _heightOffset * 0.5f;
        if (_bounce)
        {
            _repeatRange = Mathf.PI;
        }
        else
        {
            _heightOffset = _heightOffset / 2f;
        }
    }

    private void Update()
    {
        //float t = Mathf.PingPong(Time.time,1f);
        //Mathf.Repeat(Time.time, 2);
        _t += Time.deltaTime * _speed;
        _t = Mathf.Repeat(_t, _repeatRange);
        _currentHeight = _start + Mathf.Sin(_t) * _heightOffset;
        _myTransform.localPosition = new Vector3(0f, _currentHeight, 0f);
        
    }
}
