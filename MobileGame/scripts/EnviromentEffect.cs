using UnityEngine;

public class EnviromentEffect : MonoBehaviour
{
    [SerializeField] private float jumpForce = 30f;

    [SerializeField] bool _debugJump = true;
    [SerializeField] float gravityValue = -20;
    [SerializeField] float gameSpeed = 10;
    [SerializeField] int calculateFrames = 60;

    private void OnTriggerEnter(Collider other)
    {
        PlayerStateMachine player = other.gameObject.GetComponent<PlayerStateMachine>();
        if (player != null)        
        {
            player.EffectEnviromentJump(jumpForce);
        }
    }

    private void OnDrawGizmosSelected()
    {
        if (!_debugJump) { return; }

        Gizmos.color = Color.yellow;
        float deltaT = 1f / 60f;
        Vector3 p1 = Vector3.zero;
        Vector3 p2 = transform.position;
        float currentVelocityY = 0f;
        float lastVelocityY = 0f;

        for (int i = 0; i < calculateFrames; i++) 
        {
            currentVelocityY = (jumpForce + (gravityValue * (deltaT * i))) * (deltaT);
            p1.y = p2.y + currentVelocityY;
            p1.z = transform.position.z + (gameSpeed * deltaT) * i;
            Gizmos.DrawLine(p1, p2);
            p2 = p1;
            lastVelocityY = currentVelocityY;
        }
    }
}
