using System;
using UnityEngine;

public class SectionTrigger : MonoBehaviour
{
    public static event Action OnSectionTrigger;

    private bool _isActive = true;

    private void OnTriggerEnter(Collider other)
    {
        if (_isActive == false) { return; }

        if(other.GetComponent<Player>() != null)
        {
            OnSectionTrigger?.Invoke();
            _isActive = false;
        }
    }
}
