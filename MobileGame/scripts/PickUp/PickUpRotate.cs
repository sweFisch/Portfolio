using UnityEngine;

public class PickUpRotate : MonoBehaviour
{
    [SerializeField] private float _rotationSpeed = 180f;

    [Tooltip("Takes the gameobject Transform if none is choosen")]
    [SerializeField] Transform myTransform;

    private void Start()
    {
        if(myTransform == null)
        {
            myTransform = transform;
        }
    }

    void Update()
    {
        myTransform.Rotate(Vector3.up, (_rotationSpeed * Time.deltaTime));
    }
}
