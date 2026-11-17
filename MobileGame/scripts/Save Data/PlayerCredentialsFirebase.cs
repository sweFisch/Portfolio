using Firebase;
using Firebase.Auth;
//using Firebase.Database;
using Firebase.Extensions;
using UnityEngine;

public class PlayerCredentialsFirebase : MonoBehaviour, IPlayerCredentials
{
    private FirebaseAuth _auth;
    //private FirebaseDatabase _db;

    public void RegisterUser(string email, string password)
    {
        FirebaseApp.CheckAndFixDependenciesAsync().ContinueWithOnMainThread(task =>
        {
            if (task.Exception != null)
                Debug.LogError(task.Exception);

            _auth = FirebaseAuth.DefaultInstance;
            //_db = FirebaseDatabase.DefaultInstance;

            if (_auth.CurrentUser == null)
            {

            }

        });
        Debug.Log("Starting Registration");
        _auth.CreateUserWithEmailAndPasswordAsync(email, password).ContinueWithOnMainThread(task =>
        {
            if (task.Exception != null)
            {
                Debug.LogWarning(task.Exception);
            }
            else
            {
                FirebaseUser newUser = task.Result.User;
                Debug.LogFormat("User Registerd: {0} ({1})",
                  newUser.DisplayName, newUser.UserId);
            }
        });
    }

    public void SignInUser(string email, string password)
    {
        FirebaseApp.CheckAndFixDependenciesAsync().ContinueWithOnMainThread(task =>
        {
            if (task.Exception != null)
                Debug.LogError(task.Exception);

            _auth = FirebaseAuth.DefaultInstance;
            //_db = FirebaseDatabase.DefaultInstance;

            if (_auth.CurrentUser == null)
            {
            }

            _auth.SignInWithEmailAndPasswordAsync(email, password).ContinueWithOnMainThread(task =>
            {
                if (task.Exception != null)
                {
                    Debug.LogWarning(task.Exception);
                }
                else
                {
                    FirebaseUser newUser = task.Result.User;
                    Debug.LogFormat("User signed in successfully: {0} ({1})",
                      newUser.DisplayName, newUser.UserId);
                }
            });
        });
    }

    public void SignOutUser()
    {
        FirebaseApp.CheckAndFixDependenciesAsync().ContinueWithOnMainThread(task =>
        {
            if (task.Exception != null)
                Debug.LogError(task.Exception);

            _auth = FirebaseAuth.DefaultInstance;
            //_db = FirebaseDatabase.DefaultInstance;

            if (_auth.CurrentUser == null)
            {
            }

            _auth.SignOut();
            Debug.Log("User signed out");
        });
    }

    public void UpdateProfileInformation(string username)
    {
        UpdateUserInformation(username, "1");
    }

    public void UpdateUserInformation(string displayName, string photoUrl)
    {
        // https://firebase.google.com/docs/auth/unity/manage-users

        FirebaseApp.CheckAndFixDependenciesAsync().ContinueWithOnMainThread(task =>
        {
            if (task.Exception != null)
                Debug.LogError(task.Exception);

            _auth = FirebaseAuth.DefaultInstance;
            //_db = FirebaseDatabase.DefaultInstance;

            if (_auth.CurrentUser == null)
            {
                Debug.Log("Not logged in Cant change name");
                return;
            }

            Firebase.Auth.FirebaseUser user = _auth.CurrentUser;
            if (user != null)
            {
                Firebase.Auth.UserProfile profile = new Firebase.Auth.UserProfile
                {
                    //DisplayName = "Jane Q. User",
                    DisplayName = displayName,
                    //PhotoUrl = new System.Uri("https://example.com/jane-q-user/profile.jpg"),
                    PhotoUrl = new System.Uri("https://example.com/" + photoUrl + ".jpg"),
                };
                user.UpdateUserProfileAsync(profile).ContinueWith(task => {
                    if (task.IsCanceled)
                    {
                        Debug.LogError("UpdateUserProfileAsync was canceled.");
                        return;
                    }
                    if (task.IsFaulted)
                    {
                        Debug.LogError("UpdateUserProfileAsync encountered an error: " + task.Exception);
                        return;
                    }

                    Debug.Log("User profile updated successfully.");
                });
            }
        });

    }

}