using System;
using System.Collections;
using UnityEngine;

public class PlayerAttack : MonoBehaviour
{
    public static Action OnAttack;

    [SerializeField] Transform weaponHolder;
    [SerializeField] GameObject weaponVis;
    [SerializeField] LayerMask layerMask;

    Coroutine AttackCoroutine = null;

    private void Update()
    {
        if (InputCollector.TapWasPressed)
        {
            Attack();
        }
    }
    private void Attack()
    {
        if (AttackCoroutine == null)
        {
            OnAttack?.Invoke();
            StartCoroutine(PlayAnimationCO());
        }
    }

    IEnumerator PlayAnimationCO()
    {
        weaponVis.transform.localPosition = Vector3.zero + Vector3.up * 0.3f;
        weaponVis.transform.localRotation = Quaternion.Euler(-60f, 0f, 0f);
        weaponVis.SetActive(true);

        float timer = 0f;
        while (timer < 0.2f)
        {
            timer += Time.deltaTime;

            DetectAttackable();
            yield return null;
            weaponVis.transform.Rotate(540f * Time.deltaTime, 0f, 0f);
            weaponVis.transform.Translate(0f, -Time.deltaTime * 2f, 0f);

        }
        yield return null;
        weaponVis.SetActive(false);

        AttackCoroutine = null;
    }

    private void DetectAttackable()
    {
        //Debug.DrawLine(weaponHolder.position, weaponHolder.position + Vector3.forward * 2f, Color.yellow, 2f);

        //if (Physics.Raycast(weaponHolder.position, Vector3.forward, out RaycastHit hit, 2f, layerMask, QueryTriggerInteraction.Ignore))
        //{

        if(Physics.SphereCast(weaponHolder.position, 0.4f, Vector3.forward, out RaycastHit hit, 2f, layerMask, QueryTriggerInteraction.Ignore))
        {
            Attackable attackableObj = hit.collider.GetComponent<Attackable>();
            if (attackableObj != null)
            {
                //print($"attacked {attackableObj.name}");
                attackableObj.TakeDamage();
            }
        }
    }
}
