using UnityEngine;

public class GameCamera : MonoBehaviour
{

    [SerializeField] Transform _playerTransform;
    [SerializeField] float smoothDampTimeXZ = 0.5f;
    [SerializeField] float smoothDampTimeY = 0.5f;
    [SerializeField] float smoothDampTimeYFrameSpeed = 0.25f;

    [SerializeField] bool _isClampedToHeight = true;

    [SerializeField] float _playerOvershootAllowed = 0.5f;
    [SerializeField] float _playerFasterSmoothTimeYBufferFrame = 0.5f;

    private Vector3 _offsetToPlayer;

    private Vector3 _positionTargetXZ;
    private Vector3 _currentXZVelocity;

    float _playerYpos;

    float _followYVelocity;

    private void Start()
    {
        _offsetToPlayer = transform.position - _playerTransform.position;
    }

    private void LateUpdate()
    {
        //_positionTargetXZ = new Vector3(_playerTransform.position.x, 0f, _playerTransform.position.z) + _offsetToPlayer;
        Vector3 transformXY = transform.position;
        transformXY.y = 0f;
        _positionTargetXZ = new Vector3(_playerTransform.position.x+ _offsetToPlayer.x, 0f, _playerTransform.position.z + _offsetToPlayer.z);

        transformXY = Vector3.SmoothDamp( transformXY, _positionTargetXZ,ref _currentXZVelocity , smoothDampTimeXZ);


        // Handle Y lerp when jumping on a higher plattform
        // Allowed space in y is current offset to 0f - the player can be at the Horizon of the camera
        // Something close to a Soft zone 
        _playerYpos = _playerTransform.position.y;
        float targetY = _playerYpos + _offsetToPlayer.y; // the targeted offset depending on player
        float cameraYpos = this.transform.position.y;

        float allowedMinRange = _playerYpos; // the minimum allowed y pos - same as player
        
        if (cameraYpos < allowedMinRange + _playerFasterSmoothTimeYBufferFrame 
            || cameraYpos > targetY + _playerFasterSmoothTimeYBufferFrame)
        {
            //print($"{Time.frameCount} - extra fast smooth cameraYPos : {cameraYpos}");
            cameraYpos = Mathf.SmoothDamp(cameraYpos, targetY, ref _followYVelocity, smoothDampTimeYFrameSpeed);
        }
        else
        {
            cameraYpos = Mathf.SmoothDamp(cameraYpos, targetY, ref _followYVelocity, smoothDampTimeY);
        }

        // Hard limit of player Follow - on screen
        if (_isClampedToHeight)
        {
            cameraYpos = Mathf.Clamp(cameraYpos, allowedMinRange, targetY + _playerOvershootAllowed);
        }

        // Update Camera position
        transform.position = transformXY + Vector3.up * cameraYpos;

        
        // Add Camera Shake
    }
}
