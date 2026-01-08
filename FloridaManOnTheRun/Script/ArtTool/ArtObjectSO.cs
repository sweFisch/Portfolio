using UnityEngine;


[CreateAssetMenu(fileName ="ArtObjectSO",menuName = "ArtTool/ArtObjectSO")]
public class ArtObjectSO : ScriptableObject
{
    public GameObject _artObjectPrefab;
    public float yOffset = 0f;
    public float xSpacing = 1f;

}
