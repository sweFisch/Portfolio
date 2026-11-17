using UnityEngine;
using UnityEngine.InputSystem;

/// <summary>
/// Simple script to get OnMouseDown to work with new input system,
/// Send a raycast out and on hit Send Message
/// </summary>
public class MouseScreenRayCaster : MonoBehaviour
{
    Camera _camera;



    void Start()
    {
        _camera = Camera.main;
    }

    void Update()
    {
        if (Mouse.current.leftButton.wasPressedThisFrame) 
        {
            Vector2 mouseScreenPos = Mouse.current.position.ReadValue();
            //print("Raycast screenPos : " + mouseScreenPos);

            Ray screenRay = _camera.ScreenPointToRay(mouseScreenPos);

            if (Physics.Raycast(screenRay, out RaycastHit hit ,100f)) 
            {
                hit.collider.SendMessage("OnMouseDown");
            }
        }    
    }
}
