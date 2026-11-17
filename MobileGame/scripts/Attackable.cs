using UnityEngine;

public class Attackable : MonoBehaviour
{
    [SerializeField] GameObject rootGameObject;
    public void TakeDamage()
    {
        RemoveDanger removeDanger = gameObject.GetComponent<RemoveDanger>();
        if (removeDanger != null) 
        {
            SoundManager.Instance.PlayEnemyTakeDamage(transform.position);
            removeDanger.DestroyDanger();
        }
        else
        {
            Debug.LogWarning("Missing destroy danger ???");
        }
    }
}
