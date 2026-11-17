using UnityEngine;
using UnityEngine.SceneManagement;

public class UIPauseScreen : MonoBehaviour
{

    public void ReLoadScene()
    {
        UIManager.Instance.ReloadCurrentScene();
    }
}
