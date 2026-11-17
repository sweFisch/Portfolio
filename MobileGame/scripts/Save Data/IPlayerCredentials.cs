public interface IPlayerCredentials
{
    public void RegisterUser(string email, string password);

    public void SignInUser(string email, string password);
    
    public void SignOutUser();

    public void UpdateProfileInformation(string username);

}
