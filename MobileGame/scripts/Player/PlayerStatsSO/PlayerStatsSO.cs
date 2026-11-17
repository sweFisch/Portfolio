using UnityEngine;

[CreateAssetMenu(fileName =("PlayerStatsSO"), menuName=("Player/PlayerStats"))]
public class PlayerStatsSO : ScriptableObject
{
    [Header("Move")]
    public float _laneSize = 1.5f;
    public float moveCoolDown = 0.2f;
    public float moveXSpeed = 2f;

    [Header("JumpStats")]
    public float jumpCooldown = 0.2f;
    public float jumpForce = 5f;
    public float powerJumpForce = 10f;
    public float gravity = -10f;
    
    [Header("Fast Falling")]
    public float fastFallDownForce = -20f;
    public float fastFallInputWaitOnExternalJump = 0.25f;

    [Header("DuckTime")]
    public float RollTime = 0.5f;

    [Header("Stunned")]
    public float stunnedCoolDown = 0.3f;

    [Header("CollisionChecks")]
    public float groundCheckDistance = 0.2f;
    public LayerMask groundLayer;
    public float colliderHeightRun = 1.4f;
    public float colliderHeightRoll = 0.6f; // has to be radius * 2 minimum
    public float capsuleRadius = 0.3f;
    public float sideCheckDistance = 0.5f;

    [Header("Debug")]
    public bool _debugJump = false;
    public float _debugGameSpeed = 10f;
    public int _debugFrames = 60;

}
