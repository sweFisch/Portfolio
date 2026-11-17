using System.Collections;
using UnityEngine;

public class PowerSheild : PowerUp
{
    // Saves your life for one hit, explode away all danger in radius
    // Implemented as a check in the PlayerMachine on Front collisions and right before Death in a stunned state.
    // Use PowerUp is called in those cases
    float radiusOfRemoveDanger = 100f;

    [SerializeField] GameObject powerVisualSheild;

    public override void ActivatePowerUp()
    {
        powerVisualSheild.SetActive(true);
        base.ActivatePowerUp();
    }

    public override void DeactivatePowerUp()
    {
        powerVisualSheild.SetActive(false);

        base.DeactivatePowerUp();
    }

    public override void UsePowerUp()
    {
        //print($"--- Use the power Of the Shield ! --- {Time.frameCount}");
        Collider[] colliders = Physics.OverlapSphere(transform.position, radiusOfRemoveDanger);
        foreach(Collider other in colliders)
        {
            RemoveDanger removeDanger = other.GetComponent<RemoveDanger>();
            if (removeDanger != null)
            {
                removeDanger.DestroyDanger();
            }
        }

        DeactivatePowerUp();
    }

    //private IEnumerator RemoveAllDanger(Collider[] dangerObjects)
    //{
    //    // Stagger the removal for performance? 
    //    yield return null;
    //}
}
