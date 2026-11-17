using UnityEngine;

public class MoveForward : MonoBehaviour
{
    [SerializeField] float speed = 10f;

    void Start()
    {
        
    }

    void Update()
    {
        transform.Translate(Vector3.forward * speed * Time.deltaTime);
    }
}
