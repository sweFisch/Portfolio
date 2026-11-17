using UnityEditor;
using UnityEngine;

public abstract class PlayerState
{

    public abstract void EnterState();
    public abstract void ExitState();
    public abstract void Update();
    public abstract void FixedUpdate();

}
